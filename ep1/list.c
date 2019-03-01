#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdio.h>
#include "list.h"

a_list a_list_create(int n) {
    a_list list = malloc(sizeof*list);
    list->maxSize = n;
    list->size = 0;
    list->array = calloc(n , (sizeof (void*)));
    pthread_mutex_init(&(list->mutex), NULL);
    return list;
}

void a_list_add(a_list list, void *item) {
    void **copy;
    int i;
    
    pthread_mutex_lock(&(list->mutex));
    
    int pos = list->size;
    if (pos == list->maxSize) {
        list->maxSize = pos * 2;
        copy = calloc(list->maxSize , (sizeof (void*)));
        for(i=0;i<list->size;i++) {
            copy[i] = list->array[i];
        }
        free(list->array);
        list->array = copy;
    }

    list->array[pos] = item;
    list->size = list->size + 1;
        
    pthread_mutex_unlock(&(list->mutex));
}

/*
 * Remove o ultimo elemento da lista e retorna ele
 */
void* a_list_pop(a_list list) {
    void *element;
    
    pthread_mutex_lock(&(list->mutex));
    
    if(list->size == 0) {
        pthread_mutex_unlock(&(list->mutex));
        return NULL;
    }
    
    
    
    list->size = list->size - 1;
    element = list->array[list->size];
    //printf("getting: %s\n", (char*) element);
    pthread_mutex_unlock(&(list->mutex));
    return element;
}

/*
 * Adiciona a cópia de uma string na lista
 */
void a_list_stradd(a_list list, char *str) {
    char *copy;
    int len;
    
    len = strlen(str);
    copy = calloc(len + 1, sizeof(char));
    strcpy(copy, str);
    a_list_add(list, copy);
}

void a_list_free(a_list files) {
    free(files->array);
    pthread_mutex_destroy(&(files->mutex));
    free(files);
}

