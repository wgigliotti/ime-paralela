#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dir.h"

t_files createFiles(int n) {
    t_files files = malloc(sizeof*files);
    files->quantidadeMaxima = n;
    files->quantidade = 0;
    files->arquivos = malloc(n * (sizeof (char*)));
    return files;
}

void adicionaArquivo(t_files files, char *arquivo) {
    char **copia;
    int i;
    int pos = files->quantidade;
    if (pos == files->quantidadeMaxima) {
        files->quantidadeMaxima = pos * 2;
        copia = malloc(files->quantidadeMaxima * (sizeof (char*)));
        for(i=0;i<files->quantidade;i++) {
            copia[i] = files->arquivos[i];
        }
        free(files->arquivos);
        files->arquivos = copia;
    }

    files->arquivos[pos] = malloc((1 + strlen(arquivo)) * sizeof (char));
    files->quantidade++;
    strcpy(files->arquivos[pos], arquivo);
}

void free_t_files(t_files files) {
    int i;

    for (i = 0; i < files->quantidade; i++) {
        free(files->arquivos[i]);
    }
    free(files->arquivos);
    free(files);
}

void print_t_files(t_files files) {
    int i;
    printf("t_files size %d / %d \n", files->quantidade, files->quantidadeMaxima);
    for (i = 0; i < files->quantidade; i++) {
        printf("> %s\n", files->arquivos[i]);
    }
}
