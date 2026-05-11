// para usar funções do linux
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define Read_Saida  0
#define Write_Saida 1
// variavel para controle dos pipes

int main(void)
{
    char input[1024];

    while (1){

        // prompt
        printf("$");
        fgets(input, 1024, stdin);

        // remove o \n do final
        input[strcspn(input, "\n")] = '\0';

        //verifica se o imput esta vazio
        if (strcmp(input, "") == 0){
            continue;
        }

        if (strcmp(input, "exit") == 0){
            break;
        }

        //conta quantos "|" existem no input
        //precisa ser antes do strtok_r pois ele coloca \0 no lugar do |
        int contador_pipe = 0;
        for (int i = 0; input[i] != '\0'; i++){
            if (input[i] == '|')
                contador_pipe++;
        }

        //cria um array de pipes e inicializa cada par
        int pipes[contador_pipe][2];
        for (int i = 0; i < contador_pipe; i++){
            if (pipe(pipes[i]) == -1){
                perror("pipe");
                return 1;
            }
        }

        char *argv[64];
        char *saveptr1;
        char *segmento = strtok_r(input, "|", &saveptr1);
        int localizador = 0; //variavel para saber quais pares de comunicação de pipes

        // separa segmentos por "|"
        while (segmento != NULL){

            int argc = 0; //zerar a cada iteração
            char *saveptr2;
            char *token = strtok_r(segmento, " ", &saveptr2);
            char *output_file = NULL;
            char *input_file = NULL;

            // quebra o programa em tokens por espaço
            while (token != NULL){

                //detecção de < e >
                if (strcmp(token, ">") == 0){
                    output_file = strtok_r(NULL, " ", &saveptr2);
                }
                else if (strcmp(token, "<") == 0){
                    input_file = strtok_r(NULL, " ", &saveptr2);
                }
                else{
                    argv[argc] = token;
                    argc++;
                }

                token = strtok_r(NULL, " ", &saveptr2); // avança para o ultimo
            }

            argv[argc] = NULL;

            pid_t pid = fork();

            if (pid < 0){ //erro no fork
                fprintf(stderr, "Falha na realizacao do fork()\n");
                return 1;
            }

            if (pid == 0){ //processo filho

                //conecta stdin/stdout aos pipes corretos
                if (contador_pipe > 0){
                    if (localizador == 0) //caso seja o primeiro comando
                        dup2(pipes[localizador][Write_Saida], STDOUT_FILENO);
                    else if (localizador < contador_pipe){ //caso seja os comandos do meio
                        dup2(pipes[localizador - 1][Read_Saida], STDIN_FILENO);
                        dup2(pipes[localizador][Write_Saida], STDOUT_FILENO);
                    }
                    else //caso seja o ultimo comando
                        dup2(pipes[localizador - 1][Read_Saida], STDIN_FILENO);
                }

                if (input_file != NULL){
                    int fd_in = open(input_file, O_RDONLY);
                    //o dup2 vai garantir que a leitura venha do arquivo e não do terminal
                    dup2(fd_in, STDIN_FILENO);
                    close(fd_in);
                }

                if (output_file != NULL){
                    int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    //o dup2 vai garantir que a escrita vá para o arquivo e não para o terminal
                    dup2(fd_out, STDOUT_FILENO);
                    close(fd_out);
                }

                //fecha os pipes no filho antes de executar
                for (int i = 0; i < contador_pipe; i++){
                    close(pipes[i][Read_Saida]);
                    close(pipes[i][Write_Saida]);
                }

                execvp(argv[0], argv);
                perror("execvp");
                exit(1);
            }

            if (pid > 0) //processo pai espera o filho terminar
                waitpid(pid, NULL, 0);

            localizador++;
            segmento = strtok_r(NULL, "|", &saveptr1);
        }

        //fecha todos os pipes abertos
        for (int i = 0; i < contador_pipe; i++){
            close(pipes[i][Read_Saida]);
            close(pipes[i][Write_Saida]);
        }
    }

    return 0;
}