#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "carga.h"

int carregarInsere(ListaInsercao *lista) {
    lista->itens = NULL;
    lista->total = 0;
    lista->indiceAtual = 0;

    FILE *f = fopen(ARQ_INSERE, "rb");
    if (!f) return 0;

    int capacidade = 64;
    lista->itens = (Registro *) malloc(sizeof(Registro) * (size_t) capacidade);

    int32_t tamanho;
    while (fread(&tamanho, sizeof(int32_t), 1, f) == 1) {
        char *dados = (char *) malloc((size_t) tamanho + 1);
        if (fread(dados, sizeof(char), (size_t) tamanho, f) != (size_t) tamanho) {
            free(dados);
            break;
        }
        dados[tamanho] = '\0';

        if (lista->total >= capacidade) {
            capacidade *= 2;
            lista->itens = (Registro *) realloc(lista->itens, sizeof(Registro) * (size_t) capacidade);
        }

        parseRegistro(dados, &lista->itens[lista->total]);
        free(dados);
        lista->total++;
    }

    fclose(f);
    lista->indiceAtual = lerIndiceSalvo(ARQ_IDX_INSERE);
    if (lista->indiceAtual > lista->total) lista->indiceAtual = lista->total;
    return 1;
}

int carregarRemove(ListaRemocao *lista) {
    lista->codigos = NULL;
    lista->total = 0;
    lista->indiceAtual = 0;

    FILE *f = fopen(ARQ_REMOVE, "rb");
    if (!f) return 0;

    int capacidade = 64;
    lista->codigos = (char (*)[TAM_COD + 1]) malloc(sizeof(char[TAM_COD + 1]) * (size_t) capacidade);

    char codigo[TAM_COD + 1];
    while (fread(codigo, sizeof(char), TAM_COD, f) == TAM_COD) {
        codigo[TAM_COD] = '\0';

        if (lista->total >= capacidade) {
            capacidade *= 2;
            lista->codigos = (char (*)[TAM_COD + 1]) realloc(lista->codigos, sizeof(char[TAM_COD + 1]) * (size_t) capacidade);
        }

        memcpy(lista->codigos[lista->total], codigo, TAM_COD + 1);
        lista->total++;
    }

    fclose(f);
    lista->indiceAtual = lerIndiceSalvo(ARQ_IDX_REMOVE);
    if (lista->indiceAtual > lista->total) lista->indiceAtual = lista->total;
    return 1;
}

int lerIndiceSalvo(const char *caminhoIdx) {
    FILE *f = fopen(caminhoIdx, "r");
    if (!f) return 0;
    int indice = 0;
    if (fscanf(f, "%d", &indice) != 1) indice = 0;
    fclose(f);
    return indice;
}

void salvarIndiceSalvo(const char *caminhoIdx, int indice) {
    FILE *f = fopen(caminhoIdx, "w");
    if (!f) return;
    fprintf(f, "%d\n", indice);
    fclose(f);
}

void liberarListaInsercao(ListaInsercao *lista) {
    free(lista->itens);
    lista->itens = NULL;
    lista->total = 0;
}

void liberarListaRemocao(ListaRemocao *lista) {
    free(lista->codigos);
    lista->codigos = NULL;
    lista->total = 0;
}