#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <regex.h>
#include "list.h"
#include "grep.h"
#include "time.h"
#include <pthread.h>
#include <sys/types.h>

int accept_dir(char *dir) {
    int restricao = strcmp(dir, "..") == 0;
    restricao = (strcmp(dir, ".") == 0) || restricao;
    return restricao != 1;
}

void grep_read_dir(char *path, a_list list) {
    struct dirent *dir_entry;
    DIR *dir;
    char buffer[1200];

    dir = opendir(path);
    if (dir == NULL) {
        fprintf(stderr, "Deu ruim ao abrir a pasta %s\n", path);
        return;
    }
    while ((dir_entry = readdir(dir)) != NULL) {
        sprintf(buffer, "%s/%s", path, dir_entry->d_name);
        if (dir_entry->d_type == 8) {
            while(a_list_is_full(list)) {
                nanosleep(1);
            }
            a_list_stradd(list, buffer);
        } else if (dir_entry->d_type == 4 && accept_dir(dir_entry->d_name)) {
            grep_read_dir(buffer, list);
        }
    }
    closedir(dir);
}

void grep_read_file(char *file_path, regex_t regex, int **results, int *max_results) {
    char fline[GREP_STR_BUFFER];
    char *line;
    int count = 0, lastLine = -1, hasNewLine = 0, countMatch = 0;
    FILE *fp;

    fp = fopen(file_path, "r");
    if (fp == NULL) {
        (*results)[countMatch] = -1;
        return;
    }

    while (fgets(fline, GREP_STR_BUFFER, fp) != NULL) {
        if (hasNewLine) {
            count++;
        }

        if ((line = strchr(fline, '\n'))) {
            *line = '\0';
            hasNewLine = 1;
            if (lastLine == count) {
                continue;
            }
        } else {
            hasNewLine = 0;
        }

        if (!regexec(&regex, fline, 0, NULL, 0)) {
            //printf("%d\n", count);
            lastLine = count;
            if (countMatch == (*max_results)) {
                *max_results = 2 * countMatch;
                *results = realloc(*results, (*max_results) * sizeof (int));
            }
            (*results)[countMatch] = lastLine;
            countMatch++;
        }
    }
    fclose(fp);
    (*results)[countMatch] = -1;

}

grep_job job_create(int size) {
    grep_job job = malloc(sizeof*job);
    job->content_max_size = size;
    job->content = calloc(size + 1, sizeof (char));
    return job;
}

p_grep grep_create(a_list files, int jobs_pool_size, char *pattern) {
    p_grep grep = malloc(sizeof*grep);
    int i;
    grep->files = files;
    grep->jobs_queue = a_list_create(jobs_pool_size);
    grep->jobs_pool = a_list_create(jobs_pool_size);
    grep->total_files = files->current_size;
    grep->processed_files = 0;
    grep->pattern = pattern;

    for (i = 0; i < jobs_pool_size; i++) {
        a_list_add(grep->jobs_pool, job_create(GREP_STR_BUFFER));
    }

    pthread_mutex_init(&(grep->mutex), NULL);

    return grep;
}

void grep_free(p_grep grep) {
    a_list_free(grep->files);
    pthread_mutex_destroy(&(grep->mutex));
    free(grep);
}

void grep_print_result(p_grep grep, char *filename, int *result) {
    int i;
    pthread_mutex_lock(&(grep->mutex));

    for (i = 0; result[i] != -1; i++) {
        printf("%s %d\n", filename, result[i]);
    }

    pthread_mutex_unlock(&(grep->mutex));
}

grep_job job_get_from_pool(p_grep grep) {
    grep_job job = NULL;
    while ((job = a_list_remove(grep->jobs_pool)) == NULL) {
        nanosleep(1);
    }
    return job;
}

int grep_job_read_line(grep_job job, FILE *fp) {
    char *new_line;
    
    int pos = 0;
    while (fgets(job->content + pos  , job->content_max_size - pos, fp)) {
        new_line = strchr(job->content, '\n');

        if (new_line == NULL) {
            pos = job->content_max_size -1 ;
            int nmax = job->content_max_size * 2;
            job->content = realloc(job->content, (nmax + 1) * sizeof (char));
            job->content_max_size = nmax;

        } else {
            (*new_line) = '\0';
            return JOB_CREATE_OK;
        }
    }
    
    
    return JOB_CREATE_END;

}

int grep_read_job(grep_job job, FILE *fp, int line, grep_file file) {
    int ret = grep_job_read_line(job, fp);

    job->line_number = line;
    job->file = file;

    return ret;
}

grep_file grep_file_create(char *path) {
    grep_file file = malloc(sizeof*file);
    file->path = path;
    file->processed_lines = 0;
    file->result = a_list_create(100);

    return file;
}

int compare_result(const void * a, const void * b) {
    return ( *(unsigned long*)a - *(unsigned long*)b );
}

int finish_file(p_grep grep, grep_file file) {
    int i;
    int ret = 0;
    qsort(file->result->array, file->result->current_size, sizeof(void*), compare_result);
    pthread_mutex_lock(&(grep->mutex));
    for(i=0;i<file->result->current_size;i++) {
        printf("%s %lu\n", file->path, (unsigned long) file->result->array[i]);
    }
    grep->processed_files++;
    if(grep->processed_files == grep->total_files) {
        ret = 1;
    }
    pthread_mutex_unlock(&(grep->mutex));    
    return ret;
}

int finish_job(p_grep grep, grep_job job) {
    int ret = 0;
    grep_file file = job->file;
    
    
    pthread_mutex_lock(&(file->mutex));

    file->processed_lines++;
    
    
    if(file->processed_lines == file->total_lines) {
        ret = finish_file(grep, file);
    }
    pthread_mutex_unlock(&(file->mutex));    
    a_list_add(grep->jobs_pool, job);
    
    
    return ret;
}

int grep_read_next_file(p_grep grep) {
    char *next_file = a_list_remove(grep->files);
    //printf("Ler %s\n", next_file);
    grep_job job;
    grep_file file;
    int line = 0;
    if (next_file == NULL) {
        return GREP_FILE_END;
    }

    FILE *fp = fopen(next_file, "r");
    if (fp == NULL) {
        grep->total_files--;
        return JOB_CREATE_OK;
    }

    file = grep_file_create(next_file);

    job = job_get_from_pool(grep);
    while (grep_read_job(job, fp, line, file) == JOB_CREATE_OK) {
        line++;
        //printf("%s %d %s\n", next_file, line, job->content);
        job->file->total_lines = line + 1;
        a_list_add(grep->jobs_queue, job);
        
        job = job_get_from_pool(grep);
    }
    job->file->total_lines = line;
    
    a_list_add(grep->jobs_pool, job);
    

    return GREP_FILE_READ;

}
