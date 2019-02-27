#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dir.h"


int main(int argc, char** argv) {
   int i;
   int size = 140000;
   char **car = calloc(size , sizeof(char*));

   getchar();

   for(i=0;i<size;i++) {       
       car[i] = calloc(100000, sizeof(char));
       if(car[i] == NULL) {
           printf("Merda que não alocou memória...");
           exit(1);
       }
       strcpy(car[i], "oi, tudo bem com vc?");
       if(i%1 == 0) {
           
           printf("%d %lu %s\n", i, (unsigned long) car[i], car[i]);
           
       }
   }
   
   getchar();   
   for(i=0;i<size;i++) {
       free(car[i]);
   }
   
   getchar();      
   free(car);   
   getchar();
   
   return 0;
}

