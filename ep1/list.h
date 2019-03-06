#ifndef __list__
#define __list__ 1

#include <pthread.h>


/*
 * Implementação simples de uma lista thread safe
 */

typedef struct ARRAY_LIST *a_list;
struct ARRAY_LIST {
   int head; /* Quantidade de arquivos marcados */
   int tail;
   int is_full;
   int current_size;
   void **array; /* Lista dos Paths dos arquivos */
   int max_size; /* Quantidade Máxima de arquivos */
   pthread_mutex_t mutex;
};

/*
 * Cria um array list
 */
a_list a_list_create(int n);

/*
 * Adiciona um elemento no array List
 */
void a_list_add(a_list list, void *item);

/*
 * Free nos elementos do array List
 */
void a_list_free(a_list list);

/*
 * Remove o ultimo elemento da lista e retorna ele
 */
void* a_list_remove(a_list list);

/*
 * Adiciona a cópia de uma string na lista
 */
void a_list_stradd(a_list list, char *str);

/*
 * Devolve 1 se a lista está cheia, 0 caso contrário
 */
int a_list_is_full(a_list list);

/*
 * Devolve 1 se a lista está vazia, 0 caso contrário
 */
int a_list_is_empty(a_list list);

#endif

