#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINHA 512

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <numero_de_linhas> <caminho_do_arquivo>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Erro: O numero de linhas deve ser maior que zero.\n");
        return 1;
    }

    FILE *arq = fopen(argv[2], "r");
    if (arq == NULL) {
        perror("Erro ao abrir o arquivo.");
    }

    char **linhas = (char **)calloc(n, sizeof(char *));
    if (linhas == NULL) {
        perror("Erro ao alocar memoria.\n");
        fclose(arq);
        return 1;
    }

    char buffer[MAX_LINHA];
    int lidas = 0;
    int idx = 0;

    while (fgets(buffer, sizeof(buffer), arq) != NULL) {
        if (linhas[idx] != NULL) {
            free(linhas[idx]);
        }

        linhas[idx] = (char *)malloc(strlen(buffer) + 1);
        if (linhas[idx] == NULL) {
            perror("Erro ao alocar memoria.\n");
            fclose(arq);
            return 1;
        }
        strcpy(linhas[idx], buffer);

        idx = (idx + 1) % n;
        lidas++;
    }

    fclose(arq);

    int linhas_para_exibir = (lidas > n) ? lidas : n;
    int inicio = (lidas < n) ? 0 : idx;

    for (int i = 0; i < linhas_para_exibir; i++) {
        int pos = (inicio + i) % n;
        printf("%s", linhas[pos]);
        free(linhas[pos]);
    }

    free(linhas);
    
    return 0;
}