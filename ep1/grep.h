/* 
 * File:   dir.h
 * Author: willian
 *
 * Created on February 27, 2019, 11:09 PM
 */

#ifndef GREP_H
#define	GREP_H

typedef struct P_GREP_FILE *grep_file;
struct P_GREP_FILE {
    int total_lines; // Quantidade total de linhas no arquivo
    int processed_lines; // Quantidade de linhas já processadas
    a_list result; // Guarda o resultado 
    
    pthread_mutex_t mutex;
    char *path;
};

typedef struct P_GREP_JOB *grep_job;
struct P_GREP_JOB {
    int line_number;
    grep_file file;
    char *content;
    int content_max_size;
};

typedef struct P_GREP *p_grep;
struct P_GREP {
    a_list jobs_queue;
    a_list jobs_pool;
    
    int total_files;
    int processed_files;
    
    a_list files;    
    
    pthread_mutex_t mutex;
    char *pattern;
};

#define GREP_STR_BUFFER 20

#define JOB_CREATE_OK 0
#define JOB_CREATE_END 1

#define GREP_FILE_READ 0
#define GREP_FILE_END 1


void grep_read_dir(char *path, a_list list);

int finish_job(p_grep grep, grep_job job);

void grep_read_file(char *file_path, regex_t regex, int **results, int *max_results);

int grep_read_next_file(p_grep grep);

p_grep grep_create(a_list files, int jobs_pool_size, char *pattern);

void grep_free(p_grep grep);

void grep_print_result(p_grep grep, char *filename, int *result);

#endif	/* DIR_H */

