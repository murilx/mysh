# SO2 - Trabalho 1 - MySh

Trabalho da matéria de Sistemas Operacionais II. O objetivo era
construir um Shell que cumprisse os seguintes requisitos:

1. Exibe um *prompt* e aguarda a entrada do usuário.
2. O comando **exit** deve sair do processador de comandos.
3. Quando a entrada do usuário é o nome de um programa ele deve ser executado.
4. Quando a entrada do usuário é inválida, mostrar uma mensagem de erro.
5. Os parâmetros digitados devem ser passados para o programa/comando.
6. O prompt deve ter o formato: [MySh] **nome-de-usuario**@**host**:**dir-atual**$
7. O **dir-atual** do prompt deve mostrar o diretório home do usuário como ~
8. Implementar o comando **cd**.
9. Mostrar mensagem de erro caso o cd falhe. 
10. Ctrl-Z e Ctrl-C não devem causar parada ou interrupção do shell
11. Ctrl-D deve sair do shell.
12. O shell deve permitir o uso de pipes (ex. ls | wc -l)
13. O shell deve permitir o uso de múltiplos piles (ex. ls | grep a* | wc -l)

Todos os requisitos foram cumpridos.

## TODO
- Ter uma versão do README em inglês que redireciona pro README em pt-br.