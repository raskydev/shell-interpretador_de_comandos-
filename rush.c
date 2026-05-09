// para usar funções do linux
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    char input[1024];

    while (1){

        // prompt
        printf("$");
        fgets(input, 1024, stdin);
        // remove o \n do final
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0){

            break;
        }
        char *argv[64];
        int argc = 0;
        char *saveptr1;
        char *segmento = strtok_r(input, "|", &saveptr1);

        // separa segmentos por "|"
        while (segmento != NULL){

            char *saveptr2;
            char *token = strtok_r(segmento, " ", &saveptr2);
            char *output_file = NULL;
            char *input_file = NULL;

            // quebra o programa em tokens por espaço
            while (token != NULL){
                
                if (strcmp(token, ">") == 0){
                    output_file = strtok_r(NULL, " ", &saveptr2);
                }
                else if (strcmp(token, "<") == 0)
                {
                    input_file = strtok_r(NULL, " ", &saveptr2);
                }
                else
                {
                    argv[argc] = token;
                    argc++;
                }

                token = strtok_r(NULL, " ", &saveptr2); // avança para o ultimo
            }

            segmento = strtok_r(NULL, "|", &saveptr1);
        }

        argv[argc] = NULL;
    }

    return 0;
}
