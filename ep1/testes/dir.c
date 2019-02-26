#include <stdio.h>
#include <dirent.h>
#include <string.h>

int acceptDir(char *dir) {
   int restricao = strcmp(dir, "..") == 0;
   restricao = (strcmp(dir, ".") == 0) || restricao;
   return restricao != 1 ;
}

void goDir(char *dir) {
   struct dirent *pDirent;
   DIR *pDir;
   char buffer[1200];
   
   pDir = opendir(dir);
   if(pDir == NULL) {
      printf("Deu ruim ao abrir a pasta %s\n", dir);
      return;
   }
   while ((pDirent = readdir(pDir)) != NULL) {
      sprintf(buffer, "%s/%s", dir, pDirent->d_name);
      if(pDirent->d_type == 8) {
	 printf ("[%s - %d]\n", buffer, pDirent->d_type);
      } else if(pDirent->d_type == 4 && acceptDir(pDirent->d_name)) {
	goDir(buffer);
      }

   }
   closedir (pDir);

}

int main (int c, char *v[]) {

   if (c < 2) {
      printf ("Usage: testprog <dirname>\n");
      return 1;
   }
   goDir(v[1]);
   return 0;
}
