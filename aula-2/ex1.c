#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define ESC 0x1B

int main() {
    FILE *arq;
    char ch;

    arq = fopen("texto.txt", "w");
    if (arq == NULL) {
        printf("Erro ao criar o arquivo.\n");
        return 1;
    }

    while (1) {
        ch = _getch();

        if (ch == ESC)
            break;

        if (ch == '\r') {
            printf("\n");
            fputc('\n', arq);
        }

        else {
            printf("%c", ch);
            fputc(ch, arq);
        }
    }

    fclose(arq);

    printf("\nProcesso concluido, arquivo criado.\n");

    return 0;
}