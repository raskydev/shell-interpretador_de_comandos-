// para usar funções do linux
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Read_Saida 0
#define Write_Saida 1
// variavel para controle dos pipes

// henrrique — adicione os includes necessários aq:

int main(void)
{
    char input[1024];

    while (1)
    {

        // prompt
        printf("$");
        fgets(input, 1024, stdin);

        // remove o \n do final
        input[strcspn(input, "\n")] = '\0';

        // verifica se o imput esta vazio
        if (strcmp(input, "") == 0)
        {
            continue;
        }

        if (strcmp(input, "exit") == 0)
        {

            break;
        }

        char *argv[64];

        //conte quantos "|" existem no input
        int contador_pipe = 0;

        for (int i = 0; input[i] != '\0'; i++)
        {
            if (input[i] == '|')
            {
            contador_pipe++;
            }
        }
        // contar antes strtok_r coloca \0 no lugar do primeiro |

        char *saveptr1;
        char *segmento = strtok_r(input, "|", &saveptr1);  

        //crie um array de pipes: int pipes[n_pipes][2]
        int pipes[contador_pipe][2];

        for (int i = 0; i < contador_pipe; i++)
        {
            if (pipe(pipes[i]) == -1 )
            {
                perror("Falha ao criar o pipe");
                return 0; //return 0 para encerr a execuçao do while principal
            }
        }
        //chame pipe() para cada par*/

        int localizador = 0; //variavel para saber quais pares de comunicaçao de pipes

        // separa segmentos por "|"
        while (segmento != NULL)
        {
            int argc = 0; //zerar a cada interaçao
            char *saveptr2;
            char *token = strtok_r(segmento, " ", &saveptr2);
            char *output_file = NULL;
            char *input_file = NULL;

            // quebra o programa em tokens por espaço
            while (token != NULL)
            {

                // detecção de < e >
                if (strcmp(token, ">") == 0)
                {

                    // stdout vai para este arquivo via open() + dup2()
                    output_file = strtok_r(NULL, " ", &saveptr2);
                }
                else if (strcmp(token, "<") == 0)
                {
                    // stdin vem deste arquivo via open() + dup2()
                    input_file = strtok_r(NULL, " ", &saveptr2);
                }
                else
                {
                    argv[argc] = token;
                    argc++;
                }

                token = strtok_r(NULL, " ", &saveptr2); // avança para o ultimo
            }

            argv[argc] = NULL;
            /*henrique — execute o comando aqui:
                pid_t pid = fork()
                se pid == 0 (filho):
                se input_file != NULL  → open() + dup2() para stdin
                se output_file != NULL → open() + dup2() para stdout
                execvp(argv[0], argv)
                se pid > 0 (pai):
                wait() ou waitpid()*/

                //////

             /*kaio — dentro do filho, antes do execvp:
                 dup2() para conectar stdin/stdout aos pipes corretos
               no pai:
                 feche as pontas do pipe que o pai não usa*/ 
                if (pid == 0)
                {
                    //caso seja o primeiro comando
                    if (localizador == 0)
                    {
                        dup2(pipes[localizador][Write_Saida],STDOUT_FILENO);
                    }

                    //caso seja os comandos do meio
                    else if (localizador < contador_pipe)
                    {
                        dup2(pipes[localizador-1][Read_Saida], STDIN_FILENO);
                        dup2(pipes[localizador][Write_Saida], STDOUT_FILENO);
                    }
                    
                    //caso seja o ultimo comando
                    else if (localizador == contador_pipe)
                    {
                        dup(pipes[localizador - 1][Read_Saida], STDIN_FILENO);
                    }
                    
                }
                
            localizador++;
            segmento = strtok_r(NULL, "|", &saveptr1);
        }
        
        //kaio— após o while, feche todos os pipes abertos
        for (int i = 0; i < contador_pipe; i++)
        {
            close(pipes[i][Read_Saida]);
            close(pipes[i][Write_Saida]);
        }
        

    }

    return 0;
}
