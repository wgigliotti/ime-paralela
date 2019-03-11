/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   alt_pgrep.c
 * Author: willian
 *
 * Created on 11 de Março de 2019, 13:11
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "list.h"
#include "grep.h"
#include "parallel_file.h"


typedef struct GREP_PARAM *grep_p;

struct GREP_PARAM {
    char *pattern;
    pthread_mutex_t mutex;
    a_list files;
    int running;
    char *path;
};

void *run_grep_file(char *file, void *parameters, void *thread_var) {
    grep_p grep = (grep_p) parameters;
    regex_t *regex = (regex_t*) thread_var;
    int size = 1000;
    int *resultados = calloc(size, sizeof (int));

    grep_file(file, *regex, &resultados, &size);
    grep_print_result(grep->mutex, file, resultados);

    free(resultados);

    return NULL;
}

void *before_grep(void *parameters) {
    grep_p grep = (grep_p) parameters;
    regex_t *regex = malloc(sizeof (regex_t));

    if (regcomp(regex, grep->pattern, 0)) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

    return regex;
}

void *after_grep(void *parameters, void *thread_var) {
    regex_t *regex = (regex_t*) thread_var;

    regfree(regex);
    free(regex);
    return NULL;
}

void *add_files_process(void *process) {
    grep_p grep = (grep_p) process;
    grep_read_dir(grep->path, grep->files);
    grep->running = 0;
    
    return NULL;
}

void run_grep(int size, char *reg, char *path) {
    pthread_t add_files;

    grep_p grep = malloc(sizeof*grep);

    grep->pattern = reg;
    grep->files = a_list_create(1000);
    grep->path = path;
    grep->running = 1;

    pthread_mutex_init(&(grep->mutex), NULL);

    if (pthread_create(&(add_files), NULL, add_files_process, grep)) {
        fprintf(stderr, "Error creating thread\n");
        return;
    }

    parallel_file_run(size, run_grep_file, before_grep, after_grep, grep->files, &grep->running, grep);

    if (pthread_join(add_files, NULL)) {
        fprintf(stderr, "Error joining thread\n");
        return;
    }

    pthread_mutex_destroy(&(grep->mutex));
    a_list_free(grep->files);
    
    free(grep);
}

/*
 * 
 */
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

