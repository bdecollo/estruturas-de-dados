#include "sistema-cadastro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char *arq = "clientes.bin";

    Cliente c1 = {"13892106924", "Bento", "Silva", "11999998888", "Sao Paulo"};
    Cliente c2 = {"12857810235", "Raissa", "Goncalves", "21988887777", "Rio de Janeiro"};
    Cliente c3 = {"12395967246", "Jose", "Silva", "31977776666", "Belo Horizonte"};

    inserir_ordenado(arq, &c1);
    inserir_ordenado(arq, &c2);
    inserir_ordenado(arq, &c3);

    printf("--- Lista de Clientes ---\n");
    imprimir_clientes(arq);

    Indice idx = construir_indice(arq);
    Cliente resultado;
    
    if (busca_binaria_indice(arq, &idx, "11122233344", &resultado)) {
        printf("\nCliente encontrado: %s %s (Cidade: %s)\n", resultado.nome, resultado.sobrenome, resultado.cidade);
    } else {
        printf("\nCliente nao encontrado.\n");
    }

    liberar_indice(&idx);
    return 0;
}