#include "parallel_file.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "time.h"

void *parallel_runner(void *parameter) {
    p_file runner = ((p_file) parameter);
    char *file; 

    void *thread_parameter = runner->before(runner->parameters);
    
    
    while (1) {
        file = a_list_remove(runner->files);
        if(file == NULL) {
            if(*(runner->running)) {
                nanosleep(1);
                continue;
            } 
            break;
        }
        
        runner->routine(file, runner->parameters, thread_parameter);
        free(file);
    }
    
    runner->after(runner->parameters, thread_parameter);
    
    return NULL;

}

int parallel_file_run(int size, void *(*routine) (char *, void *, void*), void *(*before) (void*), void *(*after) (void*, void*), a_list files, int *running, void *parameters) {
    pthread_t *inc_x_thread = calloc(size, sizeof (pthread_t));
    int i;
    memset(inc_x_thread, 0, size * sizeof (pthread_t));

    p_file p_file = malloc(sizeof *p_file);
    p_file->before = before;
    p_file->after = after;
    p_file->running = running;
    p_file->files = files;
    p_file->parameters = parameters;
    p_file->routine = routine;


    for (i = 0; i < size; i++) {

        if (pthread_create(&(inc_x_thread[i]), NULL, parallel_runner, p_file)) {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }


    /* wait for the second thread to finish */
    for (i = 0; i < size; i++) {
        if (pthread_join(inc_x_thread[i], NULL)) {
            fprintf(stderr, "Error joining thread\n");
            return 1;
        }
    }

    return 0;
}
