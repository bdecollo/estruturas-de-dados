#ifndef SISTEMA_CADASTRO_H
#define SISTEMA_CADASTRO_H

#include <stdio.h>

#define TAM_CPF 15
#define TAM_CAMPO 100

typedef struct {
    char cpf[TAM_CPF];
    char nome[TAM_CAMPO];
    char sobrenome[TAM_CAMPO];
    char telefone[TAM_CAMPO];
    char cidade[TAM_CAMPO];
} Cliente;

typedef struct {
    long *offsets;
    int quantidade;
} Indice;

// Leitura e escrita de registros
int pega_campo(FILE *f, char *destino, int max_len);
int pega_registro(FILE *f, Cliente *cli);
void grava_registro(FILE *f, const Cliente *cli);

// Operações no arquivo
int inserir_ordenado(const char *nome_arquivo, const Cliente *novo);
int remover_registro(const char *nome_arquivo, const char *cpf);
int atualizar_registro(const char *nome_arquivo, const char *cpf, const char *novo_nome, const char *novo_sobrenome, const char *novo_telefone, const char *nova_cidade);
void merge_arquivos(const char *arq1, const char *arq2, const char *arq_final);
void imprimir_clientes(const char *nome_arquivo);

// Operações de índice e busca binária
Indice construir_indice(const char *nome_arquivo);
void liberar_indice(Indice *idx);
int busca_binaria_indice(const char *nome_arquivo, const Indice *idx, const char *cpf, Cliente *resultado);

#endif