#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/queue.h>

#define CWD_SIZE 100

int main() {
    do {
        char user_input[100];
        
        // O prompt no formato expecificado em (6)
        printf("[MySh] %s@%s:%s$ ", getenv("USER"), getenv("HOSTNAME"), getcwd(NULL, CWD_SIZE));
        fgets(user_input, sizeof(user_input), stdin);

        
        
    } while(1);
    return 0;
}
