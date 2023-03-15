#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define CWD_SIZE 100

char **split_text(char *str, const char *delim);

int main() {
    char user_input[1000];
    char **arguments = NULL;
    int status_code = 0;
    
    do {
        // O prompt no formato expecificado em (6)
        printf("[MySh] %s@%s:%s$ ", getenv("USER"), getenv("HOSTNAME"), getcwd(NULL, CWD_SIZE));
        fgets(user_input, sizeof(user_input), stdin);

        // Retira o possível '\n' que existe no final da string
        if(user_input[strlen(user_input)-1] == '\n')
            user_input[strlen(user_input)-1] = '\0';
        
        // Divide a entrada recebida em um vetor de strings
        arguments = split_text(user_input, " ");
        
        // Verifica se foi possível dividir os valores de entrada
        if(arguments == NULL)
            exit(-1);

        /* ----- Comandos 'especiais' ----- */
        if(strcmp(arguments[0], "exit") == 0)
            break;

        if(strcmp(arguments[0], "cd") == 0) {
            if(arguments[1] != NULL && !(strcmp(arguments[1], "~") == 0)) {
                status_code = chdir(arguments[1]);
                if(status_code == -1) {
                    fprintf(stderr, "Erro na execucao de 'cd' com argumento %s: %s\n",
                            arguments[1], strerror(errno));
                }
            }
            else
                chdir(getenv("HOME"));
            continue;
        }

        // Executa o comando enviado pelo usuário em um processo filho
        if(fork() == 0) {
           status_code = execvp(arguments[0], arguments);
           if(status_code == -1)
               fprintf(stderr, "Erro na execucao de %s: %s\n", arguments[0], strerror(errno));
        }
        wait(NULL); // Espera o processo filho terminar para continuar o processo pai
    } while(1);

    // Libera a memória que foi alocada
    if(arguments != NULL)
        free(arguments);
    return 0;
}

char **split_text(char *str, const char *delim) {
    char **list = NULL;
    char *token = strtok(str, delim);
    int n_spaces = 0;

    // Armazena o comando e os argumentos em uma array de strings
    while(token != NULL) {
        list = realloc(list, sizeof(char *) * ++n_spaces);
        if(list == NULL)
            exit(-1); // Falha na alocação de memória
        list[n_spaces-1] = (char *)malloc(sizeof(char *) * strlen(token) + 1);
        strcpy(list[n_spaces-1], token);
        token = strtok(NULL, delim);
    }
    // Adiciona o NULL necessário no fim da array
    list = realloc(list, sizeof(char *) * ++n_spaces);
    list[n_spaces-1] = NULL;
    return list;
}
