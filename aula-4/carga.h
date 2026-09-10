#ifndef CARGA_H
#define CARGA_H

#include "arquivo.h"

#define ARQ_INSERE       "insere.bin"
#define ARQ_REMOVE       "remove.bin"
#define ARQ_IDX_INSERE   ".idx_insere.txt"
#define ARQ_IDX_REMOVE   ".idx_remove.txt"

typedef struct {
    Registro *itens;
    int total;
    int indiceAtual;
} ListaInsercao;

typedef struct {
    char (*codigos)[TAM_COD + 1];
    int total;
    int indiceAtual;
} ListaRemocao;

int carregarInsere(ListaInsercao *lista);
int carregarRemove(ListaRemocao *lista);
int  lerIndiceSalvo(const char *caminhoIdx);
void salvarIndiceSalvo(const char *caminhoIdx, int indice);
void liberarListaInsercao(ListaInsercao *lista);
void liberarListaRemocao(ListaRemocao *lista);

#endif