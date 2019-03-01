#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <regex.h>
#include "list.h"
#include "dir.h"
#include <pthread.h>
#include <sys/types.h>

a_list initFiles(char *path, a_list files) {
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
    pthread_t threadB = pthread_self();
    
    if (regcomp(&regex, grep->pattern, 0)) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

    
    while (1) {
        file = a_list_pop(grep->files);
        if(file == NULL) {
            if(grep->active) {
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

void runGrep(char *reg, char *path, int size) {
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
    
    initFiles(path, files);
    
    printf("%d files to read\n", files->size);
    for (i = 0; i < size; i++) {
        if (pthread_create(&(inc_x_thread[i]), NULL, grep_full, &grep)) {
            fprintf(stderr, "Error creating thread\n");
            return;
        }
    }

    

    grep->active = 0;
    /* wait for the second thread to finish */
    for (i = 0; i < size; i++) {
        if (pthread_join(inc_x_thread[i], NULL)) {
            fprintf(stderr, "Error joining thread\n");
            return;
        }
    }
    printf("%d files to read\n", files->size);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Falta parâmetros!\n");
        return 3;
    }

    runGrep(argv[2], argv[1], 1);

    return 1;
}



