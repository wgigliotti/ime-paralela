#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {
   char *str;
   char *copia;
   int i;
   char *buffer[100];

   /* Initial memory allocation */
   str = (char *) malloc(100);
   copia = str;
   strcpy(str, "tutorialspoint");
   for(i=0;i<100;i++) {
	   buffer[i] = (char*)malloc(100000);

   }
   printf("String = %s,  Address = %lu\n", str, (unsigned long) str);
   printf("String = %s,  Address = %lu\n", str, (unsigned long) copia);
   /* Reallocating memory */
   str = (char *) realloc(str, 100000000);
   strcat(str, "aaa");
   strcat(copia, ".com");
   strcat(copia, "asdf");
   free(copia);
   for(i=0;i<100;i++) {
	   free(buffer[i]);
   }
   printf("String = %s,  Address = %lu\n", str, (unsigned long) str);
   printf("String = %s,  Address = %lu\n", str, (unsigned long) copia);

   free(str);
   
   return(0);
}
