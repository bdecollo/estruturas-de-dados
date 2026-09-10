#ifndef ARQUIVO_H
#define ARQUIVO_H

#include <stdio.h>
#include <stdint.h>

#define TAM_COD          3
#define TAM_NOME        50
#define TAM_SEGURADORA  50
#define TAM_TIPO        30

#define TAM_BUFFER_MAX  (TAM_COD + TAM_NOME + TAM_SEGURADORA + TAM_TIPO + 5)

#define ARQ_DADOS       "seguradoras.dat"
#define ARQ_DADOS_TMP   "seguradoras_tmp.dat"

#define MARCADOR_LIVRE '*'

typedef struct {
    char codigo[TAM_COD + 1];
    char nome[TAM_NOME + 1];
    char seguradora[TAM_SEGURADORA + 1];
    char tipo[TAM_TIPO + 1];
} Registro;

FILE *abrirOuCriarArquivo(const char *caminho);
int32_t montarBuffer(const Registro *r, char *buffer);
void parseRegistro(const char *dados, Registro *r);
int existeCodigo(FILE *f, const char *codigo);
int inserirRegistro(FILE *f, const Registro *r);
int removerRegistro(FILE *f, const char *codigo);
void compactarArquivo(FILE **f, const char *caminho);
void dumpArquivo(FILE *f);

#endif