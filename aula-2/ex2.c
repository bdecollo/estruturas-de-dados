#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_BYTES 16

void hex_dump(FILE *arquivo) {
    unsigned char buffer[MAX_BYTES];
    size_t bytes_lidos;

    while ((bytes_lidos = fread(buffer, 1, MAX_BYTES, arquivo)) > 0) {
        for (size_t i = 0; i < MAX_BYTES; i++) {
            if (i < bytes_lidos) {
                printf("%02X ", buffer[i]);
            } else {
                printf("   ");
            }
        }

        printf(" ");

        for (size_t i = 0; i < bytes_lidos; i++) {
            if (isprint(buffer[i]))
                printf("%c", buffer[i]);
            else
                printf(".");
        }

        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <file_name>\n", argv[0]);
        return 1;
    }

    FILE *arq = fopen(argv[1], "rb");
    if (arq == NULL) {
        perror("Erro ao abrir o arquivo.");
        return 1;
    }

    hex_dump(arq);

    fclose(arq);

    return 0;
}