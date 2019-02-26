#ifndef __dir__
#define __dir__ 1


typedef struct FILES *t_files;
struct FILES {
   int quantidade; /* Quantidade de arquivos marcados */
   char **arquivos; /* Lista dos Paths dos arquivos */
   int quantidadeMaxima; /* Quantidade Máxima de arquivos */
}

t_files createFiles(int n);
void adicionaArquivo(t_files files, char *arquivo);

#endif

