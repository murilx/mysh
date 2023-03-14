#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define CWD_SIZE 100

char **split_text(char *);

int main() {
    char user_input[100];
    char **arguments = NULL;
        
    do {
        // O prompt no formato expecificado em (6)
        printf("[MySh] %s@%s:%s$ ", getenv("USER"), getenv("HOSTNAME"), getcwd(NULL, CWD_SIZE));
        fgets(user_input, sizeof(user_input), stdin);

        // Divide a entrada recebida em um vetor de strings
        arguments = split_text(user_input);

        // Verifica se foi possível dividir os valores de entrada
        if(arguments == NULL)
            exit(-1);

        // Verifica por comandos especiais
        if(strcmp(arguments[0], "exit\n") == 0)
            break;

        // Executa o comando enviado pelo usuário
        execvp(arguments[0], arguments);
    } while(1);

    // Libera a memória que foi alocada
    if(arguments != NULL)
        free(arguments);
    return 0;
}

char **split_text(char *str) {
    char **list = NULL;
    char *token = strtok(str, " ");
    int n_spaces = 0;

    // Armazena o comando e os argumentos em uma array de strings
    while(token != NULL) {
        list = realloc(list, sizeof(char *) * ++n_spaces);
        if(list == NULL)
            exit(-1); // Falha na alocação de memória
        list[n_spaces-1] = (char *)malloc(sizeof(char *) * strlen(token) + 1);
        strcpy(list[n_spaces-1], token);
        token = strtok(NULL, " ");
    }
    return list;
}
