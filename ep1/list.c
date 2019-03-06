#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include "list.h"

a_list a_list_create(int n) {
    if (n < 1) {
        printf("List size should be greater than 0.\n");
        exit(-1);
    }
    a_list list = malloc(sizeof*list);
    list->head = 0;
    list->tail = 0;
    list->max_size = n;
    list->is_full = 0;
    list->current_size = 0;
    list->array = calloc(n , (sizeof (void*)));
    pthread_mutex_init(&(list->mutex), NULL);
    return list;
}

/**
 * Add a new element to the list. If the list is full, it is copied to a new
 * one that will have the double of capacity.
 * @param list
 * @param item
 */
void a_list_add(a_list list, void *item) {
    void **copy;
    int i;
   
    pthread_mutex_lock(&(list->mutex));
    
    if (list->is_full) {
//        printf("Dobrando o tamanho %d\n", list->max_size*2);
        copy = calloc(list->max_size * 2, (sizeof (void*)));
        for(i = 0; i < list->max_size; i++) {
            copy[i] = list->array[(list->head+i) % list->max_size];
        }
        free(list->array);
        list->array = copy;        
        list->head = 0;
        list->tail = list->max_size;
        list->max_size = list->max_size * 2;
        list->is_full = 0;
    }

    list->array[list->tail] = item;
    list->tail++;

    if (list->tail == list->max_size) {
        list->tail = 0;
    }
    if (list->tail == list->head) {
        list->is_full = 1;
    }
    
    list->current_size++;
    
    pthread_mutex_unlock(&(list->mutex));
}

/*
 * Remove o ultimo elemento da lista e retorna ele
 */
void* a_list_remove(a_list list) {
    void *element = NULL;
    
    pthread_mutex_lock(&(list->mutex));
    
    if (list->current_size <= 0) {
        pthread_mutex_unlock(&(list->mutex));
        return element;
    }        
    
    list->current_size--;
    element = list->array[list->head];
    
    list->head++;
    if (list->head == list->max_size) {
        list->head = 0;
    }
    list->is_full = 0;
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
    //TODO: estudar melhor como funciona esse mutex_destroy
    pthread_mutex_destroy(&(files->mutex));
    free(files);
}

int a_list_is_full(a_list list) {
    return list->is_full;    
}

int a_list_is_empty(a_list list) {
    if (list->current_size > 0) {
        return 1;
    } else {
        return 0;
    }
}
