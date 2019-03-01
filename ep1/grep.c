#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <regex.h>
#include <dirent.h>
#include "list.h"
#include "grep.h"

int accept_dir(char *dir) {
   int restricao = strcmp(dir, "..") == 0;
   restricao = (strcmp(dir, ".") == 0) || restricao;
   return restricao != 1 ;
}

void grep_read_dir(char *path, a_list list) {
    struct dirent *dir_entry;
    DIR *dir;
    char buffer[1200];

    dir = opendir(path);
    if (dir == NULL) {
        printf("Deu ruim ao abrir a pasta %s\n", path);
        return;
    }
    while ((dir_entry = readdir(dir)) != NULL) {
        sprintf(buffer, "%s/%s", path, dir_entry->d_name);
        if (dir_entry->d_type == 8) {
            a_list_stradd(list, buffer);
        } else if (dir_entry->d_type == 4 && accept_dir(dir_entry->d_name)) {
            grep_read_dir(buffer, list);
        }
    }
    closedir(dir);
}

void grep_file(char *file_path, regex_t regex, int **results, int *max_results) {
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
            if(countMatch == (*max_results)) {
                *max_results = 2 * countMatch;
                *results = realloc(*results, (*max_results) * sizeof(int));
            }
            (*results)[countMatch] = lastLine;
            countMatch++;
        }
    }
    fclose(fp);
    (*results)[countMatch] = -1;
    
} 

p_grep grep_create(a_list files, regex_t regex) {
    p_grep grep = malloc(sizeof*grep);
    grep->files = files;
    grep->regex = regex;
    grep->active = 1;
    //grep->mutex = malloc(sizeof(*grep->mutex));
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
    
    for(i=0;result[i] != -1;i++) {
        printf("%s %d\n", filename, result[i]);
    }
    
    pthread_mutex_unlock(&(grep->mutex));
}