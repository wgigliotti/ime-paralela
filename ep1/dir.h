/* 
 * File:   dir.h
 * Author: willian
 *
 * Created on February 27, 2019, 11:09 PM
 */

#ifndef GREP_H
#define	GREP_H

typedef struct P_GREP *p_grep;
struct P_GREP {
    a_list files;    
    pthread_mutex_t mutex;
    regex_t regex;
    char *pattern;
    int active;
};

#define GREP_STR_BUFFER 10000

void grep_read_dir(char *path, a_list list);

void grep_file(char *filePath, regex_t regex, int **results, int *maxResults);

p_grep grep_create(a_list files, regex_t regex);

void grep_free(p_grep grep);

void grep_print_result(p_grep grep, char *filename, int *result);

#endif	/* DIR_H */

