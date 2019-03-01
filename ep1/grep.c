#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <regex.h>
#include "list.h"
#include "grep.h"
#include <dirent.h>


int acceptDir(char *dir) {
   int restricao = strcmp(dir, "..") == 0;
   restricao = (strcmp(dir, ".") == 0) || restricao;
   return restricao != 1 ;
}

void grep_read_dir(char *path, a_list list) {
    struct dirent *pDirent;
    DIR *pDir;
    char buffer[1200];

    pDir = opendir(path);
    if (pDir == NULL) {
        printf("Deu ruim ao abrir a pasta %s\n", path);
        return;
    }
    while ((pDirent = readdir(pDir)) != NULL) {
        sprintf(buffer, "%s/%s", path, pDirent->d_name);
        if (pDirent->d_type == 8) {
            a_list_stradd(list, buffer);
        } else if (pDirent->d_type == 4 && acceptDir(pDirent->d_name)) {
            grep_read_dir(buffer, list);
        }
    }
    closedir(pDir);
}


void grep_file(char *filePath, regex_t regex, int **results, int *maxResults) {
    char fline[GREP_STR_BUFFER];
    char *line;
    int count = 0, lastLine = -1, hasNewLine = 0, countMatch = 0;
    FILE *fp;

    fp = fopen(filePath, "r");
    if (fp == NULL) {
        (*results)[countMatch] = -1;
        return;
    }

    while (fgets(fline, GREP_STR_BUFFER, fp) != NULL) {
        if (hasNewLine) {
            count++;
        }

        if ((line = strchr(fline, '\n'))) {
            *line = '\0';
            hasNewLine = 1;
            if (lastLine == count) {
                continue;
            }            
        } else {
            hasNewLine = 0;
        }
        

        if (!regexec(&regex, fline, 0, NULL, 0)) {
            //printf("%d\n", count);
            lastLine = count;
            if(countMatch == (*maxResults)) {
                *maxResults = 2 * countMatch;
                *results = realloc(*results, (*maxResults) * sizeof(int));
            }
            (*results)[countMatch] = lastLine;
            countMatch++;
        }
    }
    fclose(fp);
    (*results)[countMatch] = -1;
    
} 

p_grep grep_create(a_list files, regex_t regex) {
    p_grep grep = malloc(sizeof*grep);
    grep->files = files;
    grep->regex = regex;
    grep->active = 1;
    //grep->mutex = malloc(sizeof(*grep->mutex));
    pthread_mutex_init(&(grep->mutex), NULL);
            
    return grep;
}

void grep_free(p_grep grep) {
    a_list_free(grep->files);
    pthread_mutex_destroy(&(grep->mutex));
    free(grep);
}

void grep_print_result(p_grep grep, char *filename, int *result) {
    int i;
    pthread_mutex_lock(&(grep->mutex));
    
    for(i=0;result[i] != -1;i++) {
        printf("%s %d\n", filename, result[i]);
    }
    
    pthread_mutex_unlock(&(grep->mutex));
}