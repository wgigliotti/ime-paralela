#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <regex.h>
#include "list.h"
#include "grep.h"
#include <pthread.h>
#include <sys/types.h>

a_list init_files(char *path, a_list files) {
    grep_read_dir(path, files);
    return files;
}

void *grep_full(void *pointer) {
    p_grep grep = ((p_grep*) pointer)[0];
    regex_t regex;
    char *file;
    int size = 1000;
    int *resultados = calloc(size, sizeof (int));
    int files = 0;
//    pthread_t threadB = pthread_self();
    
    if (regcomp(&regex, grep->pattern, 0)) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

    while (1) {
        file = a_list_remove(grep->files);
        if(file == NULL) {
            if(grep->active) {
                usleep(10);
                continue;
            } 
            break;
        }
        files++;
        grep_file(file, regex, &resultados, &size);
        grep_print_result(grep, file, resultados);
        free(file);
    }
    
    //printf("%lu -> %d files\n", threadB, files);

    regfree(&regex);
    pthread_exit(NULL);

    return NULL;
}

void run_grep(int size, char *reg, char *path) {
    regex_t regex;
    p_grep grep;
    int i;
    pthread_t *inc_x_thread = calloc(size, sizeof (pthread_t));
    
    memset(inc_x_thread, 0, size * sizeof (pthread_t));
    
    if (regcomp(&regex, reg, 0)) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }
    a_list files = a_list_create(1000);
    grep = grep_create(files, regex);
    grep->pattern = reg;
    
    for (i = 0; i < size; i++) {
        if (pthread_create(&(inc_x_thread[i]), NULL, grep_full, &grep)) {
            fprintf(stderr, "Error creating thread\n");
            return;
        }
    }

    init_files(path, files);
    grep->active = 0;
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
