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

void *grep_process_only(void *pointer);

a_list init_files(char *path, a_list files) {
    grep_read_dir(path, files);
    return files;
}

void *grep_read_only(void *pointer) {
    p_grep grep = ((p_grep*) pointer)[0];

    while (grep_read_next_file(grep) == GREP_FILE_READ);
    return grep_process_only(pointer);
}

void *grep_process_only(void *pointer) {
    p_grep grep = ((p_grep*) pointer)[0];
    regex_t regex;
    grep_job job;
    if (regcomp(&regex, grep->pattern, 0)) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

    while (1) {
        job = a_list_remove(grep->jobs_queue);
        if(job == NULL) {
            if(grep->total_files == grep->processed_files) {
                break;
            }
            nanosleep(1);
            continue;
        }
        if (!regexec(&regex, job->content, 0, NULL, 0)) {
            a_list_add(job->file->result, (void*) ((unsigned long) job->line_number));
        }
        int finish = finish_job(grep, job);
        if(finish) {
            break;
        }
    }

    return NULL;

}

void run_grep(int size, char *reg, char *path) {
    regex_t regex;
    p_grep grep;
    int i = 0;
    int read_onlye = 3;
    pthread_t *inc_x_thread = calloc(size, sizeof (pthread_t));

    memset(inc_x_thread, 0, size * sizeof (pthread_t));

    if (regcomp(&regex, reg, 0)) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }
    a_list files = a_list_create(1000);
    init_files(path, files);
    grep = grep_create(files, 10000, reg);

    for (i = 0; i < read_onlye && i < size; i++) {
        if (pthread_create(&(inc_x_thread[i]), NULL, grep_read_only, &grep)) {
            fprintf(stderr, "Error creating thread\n");
            return;
        }
    }

    for (i = read_onlye; i < size; i++) {
        if (pthread_create(&(inc_x_thread[i]), NULL, grep_process_only, &grep)) {
            fprintf(stderr, "Error creating thread\n");
            return;
        }
    }

    
    /* wait for the second thread to finish */
    for (i = 0; i < size; i++) {
        if (pthread_join(inc_x_thread[i], NULL)) {
            fprintf(stderr, "Error joining thread\n");
            return;
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 4) {
        printf("Running pgrep:\n");
        printf("$ pgrep <number of threads> <regular expression> <path>:\n");
        return 3;
    }

    int max_threads = atoi(argv[1]);
    run_grep(max_threads, argv[2], argv[3]);

    return 1;
}
