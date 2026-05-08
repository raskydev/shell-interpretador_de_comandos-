// para usar funções do linux
#define _POSIX_C_SOURCE 200809L

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

int main(void){
    char input[1024];

    while (1){

    //prompt
        printf("$");

        fgets(input, 1024, stdin);

    //remove o \n do final     
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0){

            break;

        }

    } return 0;
}
 