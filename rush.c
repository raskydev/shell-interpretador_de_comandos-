// para usar funções do linux
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


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
        int argc = 0;
        char *saveptr1;
        char *segmento = strtok_r(input, "|", &saveptr1);

        /*conte quantos "|" existem no input
        //crie um array de pipes: int pipes[n_pipes][2]
        //chame pipe() para cada par*/

        // separa segmentos por "|"
        while (segmento != NULL)
        {

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

            pid_t pid = fork();

            if (pid < 0) { //erro no fork
		        fprintf(stderr, "Falha na realizacao do fork()\n");
		        return 1;
	        }

            if(pid==0){
                if(input_file != NULL){
                   int fd_in = open(input_file, O_RDONLY); //lê do arquivo input
                   dup2(fd_in, STDIN_FILENO);
                   //quando o usuário escrever ls > coisa.txt o objetivo é ler o que tá em ls e escrever em coisa.txt
                   //o dup2 vai garantir que a leitura seja escrita no arquivo e não printada no terminal
                   close(fd_in);
                }
                
                if(output_file != NULL){
                    int fd_out = open(output_file,O_WRONLY | O_CREAT | O_TRUNC,0644); //cria o arquivo file do tipo somente escrita, zera
                                                                                      //o conteúdo do arquivo ao abrir e pede permissão
                    dup2(fd_out, STDOUT_FILENO);
                    close(fd_out);
                }
                execvp(argv[0], argv);
                perror("execvp");
                exit(1);
            }

            if(pid>0){
                waitpid(pid,NULL,0);
                
            }

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

            segmento = strtok_r(NULL, "|", &saveptr1);
        }
        
        //kaio— após o while, feche todos os pipes abertos

    }

    return 0;
}
