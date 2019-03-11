/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   parallel_file.h
 * Author: willian
 *
 * Created on 11 de Março de 2019, 12:53
 */

#ifndef PARALLEL_FILE_H
#define PARALLEL_FILE_H

#include "list.h"

typedef struct P_FILE *p_file;
struct P_FILE {
    a_list files;    
    int *running;
    void *(*before) (void*);
    void *(*after) (void*, void*);
    void *(*routine) (char *, void *, void*);
    void *parameters;
};

int parallel_file_run(int size, void *(*routine) (char *, void *, void*), void *(*before) (void*), void *(*after) (void*, void*), a_list files, int *running, void *parameters);

#endif /* PARALLEL_FILE_H */

