#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "arquivo.h"

#define TAM_CABECALHO ((int32_t) sizeof(int32_t))
#define TAM_MIN_NO_LIVRE ((int32_t)(sizeof(char) + sizeof(int32_t)))

static int32_t lerCabecalho(FILE *f) {
    int32_t offsetLivre = -1;
    fseek(f, 0, SEEK_SET);
    fread(&offsetLivre, sizeof(int32_t), 1, f);
    return offsetLivre;
}

static void escreverCabecalho(FILE *f, int32_t offsetLivre) {
    fseek(f, 0, SEEK_SET);
    fwrite(&offsetLivre, sizeof(int32_t), 1, f);
}

FILE *abrirOuCriarArquivo(const char *caminho) {
    FILE *f = fopen(caminho, "r+b");
    if (f == NULL) {
        f = fopen(caminho, "w+b");
        if (f == NULL) return NULL;
        escreverCabecalho(f, -1);
        fflush(f);
    }
    return f;
}

int32_t montarBuffer(const Registro *r, char *buffer) {
    int n = sprintf(buffer, "%s#%s#%s#%s#",
                     r->codigo, r->nome, r->seguradora, r->tipo);
    return (int32_t) n;
}

void parseRegistro(const char *dados, Registro *r) {
    char copia[TAM_BUFFER_MAX + 64];
    strncpy(copia, dados, sizeof(copia) - 1);
    copia[sizeof(copia) - 1] = '\0';

    r->codigo[0] = r->nome[0] = r->seguradora[0] = r->tipo[0] = '\0';

    char *tok = strtok(copia, "#");
    if (tok) { strncpy(r->codigo, tok, TAM_COD); r->codigo[TAM_COD] = '\0'; }

    tok = strtok(NULL, "#");
    if (tok) { strncpy(r->nome, tok, TAM_NOME); r->nome[TAM_NOME] = '\0'; }

    tok = strtok(NULL, "#");
    if (tok) { strncpy(r->seguradora, tok, TAM_SEGURADORA); r->seguradora[TAM_SEGURADORA] = '\0'; }

    tok = strtok(NULL, "#");
    if (tok) { strncpy(r->tipo, tok, TAM_TIPO); r->tipo[TAM_TIPO] = '\0'; }
}

static int32_t procurarAtivoPorCodigo(FILE *f, const char *codigo, int32_t *tamanhoSlot) {
    fseek(f, 0, SEEK_END);
    int32_t fim = (int32_t) ftell(f);
    int32_t offsetAtual = TAM_CABECALHO;

    while (offsetAtual < fim) {
        fseek(f, offsetAtual, SEEK_SET);
        int32_t tamSlot;
        if (fread(&tamSlot, sizeof(int32_t), 1, f) != 1) break;

        char primeiroByte;
        if (fread(&primeiroByte, sizeof(char), 1, f) != 1) break;

        if (primeiroByte == MARCADOR_LIVRE) {
            offsetAtual += sizeof(int32_t) + tamSlot;
            continue;
        }

        char codLido[TAM_COD + 1];
        codLido[0] = primeiroByte;
        fread(codLido + 1, sizeof(char), TAM_COD - 1, f);
        codLido[TAM_COD] = '\0';

        if (strncmp(codLido, codigo, TAM_COD) == 0) {
            if (tamanhoSlot) *tamanhoSlot = tamSlot;
            return offsetAtual;
        }

        offsetAtual += sizeof(int32_t) + tamSlot;
    }
    return -1;
}

int existeCodigo(FILE *f, const char *codigo) {
    return procurarAtivoPorCodigo(f, codigo, NULL) != -1;
}

int inserirRegistro(FILE *f, const Registro *r) {
    if (existeCodigo(f, r->codigo)) {
        return 0; /* chave duplicada */
    }

    char buffer[TAM_BUFFER_MAX];
    int32_t tamNecessario = montarBuffer(r, buffer);

    int32_t offsetAnterior = -1;
    int32_t offsetAtual = lerCabecalho(f);

    int32_t offsetAchado = -1;
    int32_t tamanhoAchado = -1;
    int32_t anteriorDoAchado = -1;
    int32_t proximoDoAchado = -1;

    while (offsetAtual != -1) {
        fseek(f, offsetAtual, SEEK_SET);
        int32_t tamSlot;
        fread(&tamSlot, sizeof(int32_t), 1, f);
        char marcador;
        fread(&marcador, sizeof(char), 1, f);
        int32_t offsetProx;
        fread(&offsetProx, sizeof(int32_t), 1, f);

        if (marcador == MARCADOR_LIVRE && tamSlot >= tamNecessario) {
            offsetAchado = offsetAtual;
            tamanhoAchado = tamSlot;
            anteriorDoAchado = offsetAnterior;
            proximoDoAchado = offsetProx;
            break;
        }

        offsetAnterior = offsetAtual;
        offsetAtual = offsetProx;
    }

    if (offsetAchado != -1) {
        if (anteriorDoAchado == -1) {
            escreverCabecalho(f, proximoDoAchado);
        } else {
            fseek(f, anteriorDoAchado + (int32_t) sizeof(int32_t) + (int32_t) sizeof(char), SEEK_SET);
            fwrite(&proximoDoAchado, sizeof(int32_t), 1, f);
        }
        fseek(f, offsetAchado, SEEK_SET);
        fwrite(&tamanhoAchado, sizeof(int32_t), 1, f);
        fwrite(buffer, sizeof(char), (size_t) tamNecessario, f);
    } else {
        fseek(f, 0, SEEK_END);
        fwrite(&tamNecessario, sizeof(int32_t), 1, f);
        fwrite(buffer, sizeof(char), (size_t) tamNecessario, f);
    }

    fflush(f);
    return 1;
}

int removerRegistro(FILE *f, const char *codigo) {
    int32_t tamSlot;
    int32_t offsetAchado = procurarAtivoPorCodigo(f, codigo, &tamSlot);
    if (offsetAchado == -1) {
        return 0;
    }

    int32_t offsetPrimeiroLivreAntigo = lerCabecalho(f);

    fseek(f, offsetAchado, SEEK_SET);
    fwrite(&tamSlot, sizeof(int32_t), 1, f);
    char marcador = MARCADOR_LIVRE;
    fwrite(&marcador, sizeof(char), 1, f);
    fwrite(&offsetPrimeiroLivreAntigo, sizeof(int32_t), 1, f);

    escreverCabecalho(f, offsetAchado);

    fflush(f);
    return 1;
}

void compactarArquivo(FILE **fPtr, const char *caminho) {
    FILE *f = *fPtr;
    FILE *tmp = fopen(ARQ_DADOS_TMP, "w+b");
    if (!tmp) {
        fprintf(stderr, "Erro ao criar arquivo temporario de compactacao.\n");
        return;
    }

    escreverCabecalho(tmp, -1);

    fseek(f, 0, SEEK_END);
    int32_t fim = (int32_t) ftell(f);
    int32_t offsetAtual = TAM_CABECALHO;

    while (offsetAtual < fim) {
        fseek(f, offsetAtual, SEEK_SET);
        int32_t tamSlot;
        if (fread(&tamSlot, sizeof(int32_t), 1, f) != 1) break;

        char primeiroByte;
        if (fread(&primeiroByte, sizeof(char), 1, f) != 1) break;

        if (primeiroByte != MARCADOR_LIVRE) {
            char *dados = (char *) malloc((size_t) tamSlot + 1);
            dados[0] = primeiroByte;
            fread(dados + 1, sizeof(char), (size_t) tamSlot - 1, f);
            dados[tamSlot] = '\0';

            Registro r;
            parseRegistro(dados, &r);
            free(dados);

            char buffer[TAM_BUFFER_MAX];
            int32_t tamReal = montarBuffer(&r, buffer);
            fwrite(&tamReal, sizeof(int32_t), 1, tmp);
            fwrite(buffer, sizeof(char), (size_t) tamReal, tmp);
        }

        offsetAtual += sizeof(int32_t) + tamSlot;
    }

    fclose(f);
    fclose(tmp);

    remove(caminho);
    rename(ARQ_DADOS_TMP, caminho);

    *fPtr = fopen(caminho, "r+b");
}

void dumpArquivo(FILE *f) {
    int32_t offsetLivre = lerCabecalho(f);
    printf("---------------------------------------------------------------\n");
    printf("CABECALHO: offset do primeiro espaco livre = %d\n", offsetLivre);
    printf("---------------------------------------------------------------\n");

    fseek(f, 0, SEEK_END);
    int32_t fim = (int32_t) ftell(f);
    int32_t offsetAtual = TAM_CABECALHO;

    int totalAtivos = 0, totalLivres = 0;
    long bytesFragInterna = 0;

    while (offsetAtual < fim) {
        fseek(f, offsetAtual, SEEK_SET);
        int32_t tamSlot;
        if (fread(&tamSlot, sizeof(int32_t), 1, f) != 1) break;

        char primeiroByte;
        if (fread(&primeiroByte, sizeof(char), 1, f) != 1) break;

        if (primeiroByte == MARCADOR_LIVRE) {
            int32_t offsetProx;
            fread(&offsetProx, sizeof(int32_t), 1, f);
            printf("[offset %6d] ESPACO LIVRE   | tamanho=%-4d | proximo_livre=%d\n",
                   offsetAtual, tamSlot, offsetProx);
            totalLivres++;
        } else {
            char *dados = (char *) malloc((size_t) tamSlot + 1);
            dados[0] = primeiroByte;
            fread(dados + 1, sizeof(char), (size_t) tamSlot - 1, f);
            dados[tamSlot] = '\0';

            Registro r;
            parseRegistro(dados, &r);

            char bufferReal[TAM_BUFFER_MAX];
            int32_t tamReal = montarBuffer(&r, bufferReal);

            printf("[offset %6d] REGISTRO ATIVO | tamanho=%-4d (frag.interna=%d) | codigo=%s nome=%s seguradora=%s tipo=%s\n",
                   offsetAtual, tamSlot, tamSlot - tamReal,
                   r.codigo, r.nome, r.seguradora, r.tipo);

            bytesFragInterna += (tamSlot - tamReal);
            free(dados);
            totalAtivos++;
        }

        offsetAtual += sizeof(int32_t) + tamSlot;
    }

    printf("---------------------------------------------------------------\n");
    printf("Total de registros ativos : %d\n", totalAtivos);
    printf("Total de espacos livres   : %d\n", totalLivres);
    printf("Bytes de fragmentacao interna: %ld\n", bytesFragInterna);
    printf("Tamanho do arquivo (bytes): %d\n", fim);
    printf("---------------------------------------------------------------\n");
}