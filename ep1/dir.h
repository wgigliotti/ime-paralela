#ifndef __dir__
#define __dir__ 1


typedef struct T_FILES *t_files;
struct T_FILES {
   int quantidade; /* Quantidade de arquivos marcados */
   char **arquivos; /* Lista dos Paths dos arquivos */
   int quantidadeMaxima; /* Quantidade Máxima de arquivos */
};

t_files createFiles(int n);
void adicionaArquivo(t_files files, char *arquivo);
void free_t_files(t_files files);
void print_t_files(t_files files);

#endif

