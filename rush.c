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
    //quebra o programa em tokens
    char *token = strtok(input, " ");
    char *argv[64];
    int argc = 0;
    
        while (token != NULL){

            argv[argc] = token; 
            token = strtok(NULL, " ");
            argc++;

        }
        argv[argc] = NULL;
    }

    
    return 0;
}
 