#include "sistema-cadastro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQ_TEMP "temp.bin"

int pega_campo(FILE *f, char *destino, int max_len) {
    int c;
    int i = 0;

    c = fgetc(f);
    if (c == EOF) return 0;

    while (c != EOF && c != '|') {
        if (i < max_len - 1) destino[i++] = (char) c;
        c = fgetc(f);
    }
    destino[i] = '\0';

    return 1;
}

int pega_registro(FILE *f, Cliente *cli) {
    if (!pega_campo(f, cli->cpf, sizeof(cli->cpf))) return 0;

    pega_campo(f, cli->nome, sizeof(cli->nome));
    pega_campo(f, cli->sobrenome, sizeof(cli->sobrenome));
    pega_campo(f, cli->telefone, sizeof(cli->telefone));
    pega_campo(f, cli->cidade, sizeof(cli->cidade));
    
    return 1;
}

void grava_registro(FILE *f, const Cliente *cli) {
    fprintf(f, "%s|%s|%s|%s|%s|", cli->cpf, cli->nome, cli->sobrenome, cli->telefone, cli->cidade);
}

int inserir_ordenado(const char *nome_arquivo, const Cliente *novo) {
    FILE *f = fopen(nome_arquivo, "rb");
    FILE *tmp = fopen(ARQ_TEMP, "wb");
    Cliente atual;
    int inserido = 0;
    int duplicado = 0;

    if (!tmp) {
        if (f) fclose(f);
        return -1;
    }

    if (f) {
        while (pega_registro(f, &atual)) {
            if (!inserido) {
                int cmp = strcmp(atual.cpf, novo->cpf);
                if (cmp == 0) {
                    duplicado = 1;
                    inserido = 1;
                } else if (cmp > 0) {
                    grava_registro(tmp, novo);
                    inserido = 1;
                }
            }
            grava_registro(tmp, &atual);
        }
        fclose(f);
    }

    if (!inserido) {
        grava_registro(tmp, novo);
    }

    fclose(tmp);
    remove(nome_arquivo);
    rename(ARQ_TEMP, nome_arquivo);

    return duplicado ? 0 : 1;
}

int remover_registro(const char *nome_arquivo, const char *cpf) {
    FILE *f = fopen(nome_arquivo, "rb");
    FILE *tmp;
    Cliente atual;
    int removido = 0;

    if (!f) return 0;

    tmp = fopen(ARQ_TEMP, "wb");
    if (!tmp) {
        fclose(f);
        return 0;
    }

    while (pega_registro(f, &atual)) {
        if (!removido && strcmp(atual.cpf, cpf) == 0) {
            removido = 1;
            continue;
        }
        grava_registro(tmp, &atual);
    }

    fclose(f);
    fclose(tmp);

    if (removido) {
        remove(nome_arquivo);
        rename(ARQ_TEMP, nome_arquivo);
    } else {
        remove(ARQ_TEMP);
    }

    return removido;
}

int atualizar_registro(const char *nome_arquivo, const char *cpf, const char *novo_nome, const char *novo_sobrenome, const char *novo_telefone, const char *nova_cidade) {
    FILE *f = fopen(nome_arquivo, "rb");
    FILE *tmp;
    Cliente atual;
    int atualizado = 0;

    if (!f) return 0;
    
    tmp = fopen(ARQ_TEMP, "wb");
    if (!tmp) {
        fclose(f);
        return 0;
    }

    while (pega_registro(f, &atual)) {
        if (strcmp(atual.cpf, cpf) == 0) {
            if (novo_nome) strncpy(atual.nome, novo_nome, sizeof(atual.nome) - 1);
            if (novo_sobrenome) strncpy(atual.sobrenome, novo_sobrenome, sizeof(atual.sobrenome) - 1);
            if (novo_telefone) strncpy(atual.telefone, novo_telefone, sizeof(atual.telefone) - 1);
            if (nova_cidade) strncpy(atual.cidade, nova_cidade, sizeof(atual.cidade) - 1);
            atualizado = 1;
        }
        grava_registro(tmp, &atual);
    }

    fclose(f);
    fclose(tmp);

    if (atualizado) {
        remove(nome_arquivo);
        rename(ARQ_TEMP, nome_arquivo);
    } else {
        remove(ARQ_TEMP);
    }

    return atualizado;
}

Indice construir_indice(const char *nome_arquivo) {
    Indice idx;
    FILE *f;
    Cliente tmp;
    long pos;
    int capacidade = 16;

    idx.offsets = NULL;
    idx.quantidade = 0;

    f = fopen(nome_arquivo, "rb");
    if (!f) return idx;

    idx.offsets = malloc((size_t) capacidade * sizeof(long));

    pos = ftell(f);
    while (pega_registro(f, &tmp)) {
        if (idx.quantidade >= capacidade) {
            capacidade *= 2;
            idx.offsets = realloc(idx.offsets, (size_t) capacidade * sizeof(long));
        }
        idx.offsets[idx.quantidade++] = pos;
        pos = ftell(f);
    }

    fclose(f);
    return idx;
}

void liberar_indice(Indice *idx) {
    free(idx->offsets);
    idx->offsets = NULL;
    idx->quantidade = 0;
}

int busca_binaria_indice(const char *nome_arquivo, const Indice *idx, const char *cpf, Cliente *resultado) {
    FILE *f;
    int low, high;

    if (idx->quantidade == 0) return 0;

    f = fopen(nome_arquivo, "rb");
    if (!f) return 0;

    low = 0;
    high = idx->quantidade - 1;

    while (low <= high) {
        int meio = low + (high - low) / 2;
        Cliente atual;
        int cmp;

        fseek(f, idx->offsets[meio], SEEK_SET);
        pega_registro(f, &atual);

        cmp = strcmp(atual.cpf, cpf);
        if (cmp == 0) {
            *resultado = atual;
            fclose(f);
            return 1;
        } else if (cmp < 0) {
            low = meio + 1;
        } else {
            high = meio - 1;
        }
    }

    fclose(f);
    return 0;
}

void merge_arquivos(const char *arq1, const char *arq2, const char *arq_final) {
    FILE *f1 = fopen(arq1, "rb");
    FILE *f2 = fopen(arq2, "rb");
    FILE *final = fopen(arq_final, "wb");
    Cliente c1, c2;
    int tem1, tem2;

    if (!final) {
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return;
    }

    tem1 = f1 ? pega_registro(f1, &c1) : 0;
    tem2 = f2 ? pega_registro(f2, &c2) : 0;

    while (tem1 && tem2) {
        int cmp = strcmp(c1.cpf, c2.cpf);
        if (cmp < 0) {
            grava_registro(final, &c1);
            tem1 = pega_registro(f1, &c1);
        } else if (cmp > 0) {
            grava_registro(final, &c2);
            tem2 = pega_registro(f2, &c2);
        } else {
            grava_registro(final, &c1);
            tem1 = pega_registro(f1, &c1);
            tem2 = pega_registro(f2, &c2); 
        }
    }

    while (tem1) {
        grava_registro(final, &c1);
        tem1 = pega_registro(f1, &c1);
    }
    while (tem2) {
        grava_registro(final, &c2);
        tem2 = pega_registro(f2, &c2);
    }

    if (f1) fclose(f1);
    if (f2) fclose(f2);
    fclose(final);
}

void imprimir_clientes(const char *nome_arquivo) {
    FILE *f = fopen(nome_arquivo, "rb");
    Cliente c;
    int n = 0;

    if (!f) {
        printf("Arquivo '%s' nao existe\n", nome_arquivo);
        return;
    }

    while (pega_registro(f, &c)) {
        printf("[%d] cpf=%-11s nome=%-5s sobrenome=%-12s telefone=%-13s cidade=%s\n", ++n, c.cpf, c.nome, c.sobrenome, c.telefone, c.cidade);
    }

    if (n == 0) printf("Arquivo vazio\n");
    fclose(f);
}