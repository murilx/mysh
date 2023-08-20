#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

#define HOSTNAME_LEN 100

/* void handler(int sig) { write(STDIN_FILENO, "\n", 1); } */
void exec_pipe(char ***commands, int num_commands);
char **strspl(char *str, const char *delim);

int main() {
    char user_input[BUFSIZ];
    char **commands = NULL;      // Vetor contendo todos os comandos separados por "|"
    char ***command_list = NULL; // Vetor contendo todos um vetores de comandos formatados 
    char *cwd = NULL;            // Diretório atual
    char hostname[HOSTNAME_LEN];
    int num_commands = 0;        // Número de comandos enviados pelo usuário
    int status_code;
    int cd_command;              // Verifica se o comando cd foi utilizado

    
    // Ignora os sinais SIGINT (enviado por Ctrl+C) e SIGTSTP (enviado por Ctrl+Z)
    struct sigaction signal_ignore;
    signal_ignore.sa_handler = SIG_IGN;
    sigemptyset(&signal_ignore.sa_mask);
    signal_ignore.sa_flags = 0;

    sigaction(SIGINT, &signal_ignore, NULL);
    sigaction(SIGTSTP, &signal_ignore, NULL);
    
    do {
        // Reseta a variável cd_command
        cd_command = 0;
        
        // Caso haja $HOME no diretório atual, substitui por ~
        cwd = getcwd(NULL, 0);
        if(strstr(cwd, getenv("HOME")) != NULL) {
            int tmp = 1;
            cwd[0] = '~';
            if(strlen(getenv("HOME")) == strlen(cwd))
                cwd[1] = '\0';
            else {
                for(int i = strlen(getenv("HOME")); i < strlen(cwd); i++) {
                    char aux = cwd[i];
                    cwd[tmp++] = aux;
                }
                cwd[tmp] = '\0';
            }
        }

        // O prompt no formato expecificado em (6)
        gethostname(hostname, HOSTNAME_LEN);
        printf("[MySh] %s@%s:%s$ ", getenv("USER"), hostname, cwd);
        user_input[0] = '\0'; // "limpa" o user_input para evitar um bug com Ctrl+D
        fgets(user_input, sizeof(user_input), stdin);

        /* Faz essa verificação pra caso o usuário tenha apenas apertado
           'enter' e mais nada, ou seja, o input tenha sido apenas um '\n'
        */
        if(user_input[0] == '\n') continue;

        /* Caso o user_input seja '\0' isso indica que o usuário apertou
           Ctrl+D, que é o comando responsável por inserir um caracter de
           fim de arquivo (end of file). Nesse caso, o shell termina.
        */
        if(user_input[0] == '\0' || user_input[0] == '\b') break;
        
        // Retira o possível '\n' que existe no final da string
        if(user_input[strlen(user_input)-1] == '\n')
            user_input[strlen(user_input)-1] = '\0';

        // Divide os diferentes comandos enviados pelo usuário usando pipe
        commands = strspl(user_input, "|");

        // Verifica se foi possível dividir os valores de entrada
        if(commands == NULL)
            exit(EXIT_FAILURE);

        // Descobre a quantidade de comandos que foram passados
        num_commands = 0;
        for(num_commands; commands[num_commands] != NULL; num_commands++);
        command_list = malloc(num_commands * sizeof(char **));

        // Passa por todos os comandos executados
        // e os adiciona ao vetor de comandos formatados
        for(int i = 0; i < num_commands; i++) {
            // Divide a entrada recebida em um vetor de strings
            command_list[i] = strspl(commands[i], " ");
        
            // Verifica se foi possível dividir os valores de entrada
            if(command_list[i] == NULL)
                exit(EXIT_FAILURE);
        }

        for(int i = 0; i < num_commands; i++) {
            /* ----- Comandos 'especiais' ----- */
            if(strcmp(command_list[i][0], "exit") == 0)
                // Finaliza o shell e vai para fora do loop while
                // onde é feito o tratamento de memória
                goto exit_shell;
            
            if(strcmp(command_list[i][0], "cd") == 0) {
                cd_command = 1;
                if(command_list[i][1] != NULL && !(strcmp(command_list[i][1], "~") == 0)) {
                    status_code = chdir(command_list[i][1]);
                    if(status_code == -1) {
                        fprintf(stderr, "Erro na execucao de 'cd' com argumento %s: %s\n",
                                command_list[i][1], strerror(errno));
                    }
                }
                else
                    chdir(getenv("HOME"));
                continue;
            }
        }

        /* Verifica se o comando cd foi utilizado e, se sim,
           impede que a função exec_pipe seja executada

           Se o comando cd é utilizado em algum momento do pipe
           então apenas ele é executado, uma vez que não faz sentido
           executar outro comando além do cd no pipe
        */
        if(cd_command) continue;
  
        // Executa todos os comandos em um pipe multiplo
        exec_pipe(command_list, num_commands);
        
    } while(1);
 exit_shell:
    // Libera a memória que foi alocada
    if(commands != NULL) free(commands);
    if(cwd != NULL) free(cwd);
    for(int i = 0; i < num_commands; i++)
        if(command_list[i] != NULL) free(command_list[i]);
    if(command_list != NULL) free(command_list);
    return 0;
}

char **strspl(char *str, const char *delim) {
    char **list = NULL;
    char *token = strtok(str, delim);
    int n_spaces = 0;

    // Armazena o comando e os argumentos em uma array de strings
    while(token != NULL) {
        list = realloc(list, sizeof(char *) * ++n_spaces);
        if(list == NULL)
            exit(EXIT_FAILURE); // Falha na alocação de memória
        list[n_spaces-1] = (char *)malloc(sizeof(char *) * strlen(token) + 1);
        strcpy(list[n_spaces-1], token);
        token = strtok(NULL, delim);
    }
    // Adiciona o NULL necessário no fim da array
    list = realloc(list, sizeof(char *) * ++n_spaces);
    list[n_spaces-1] = NULL;
    return list;
}

void exec_pipe(char ***commands, int num_commands) {
    int input = 0;
    int i;
    int status;
    int fd[2];
    pid_t pid;
    
    for (i = 0; i < num_commands; i++) {
        pipe(fd);
        
        pid = fork();
        if (pid == 0) {
            dup2(input, 0);
            if (i < num_commands - 1) {
                dup2(fd[1], 1);
            }
            close(fd[0]);
            execvp(commands[i][0], commands[i]);
            perror("execvp");
            exit(1);
        } else if (pid < 0) {
            perror("fork");
            exit(1);
        }
        
        close(fd[1]);
        input = fd[0];
    }
    
    for (i = 0; i < num_commands; i++) {
        wait(&status);
    }
}
