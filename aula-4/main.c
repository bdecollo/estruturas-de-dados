#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"
#include "carga.h"

static void lerLinha(char *destino, int tamMax) {
    char bufferGrande[512];
    if (fgets(bufferGrande, sizeof(bufferGrande), stdin) == NULL) {
        destino[0] = '\0';
        return;
    }

    size_t n = strlen(bufferGrande);
    if (n > 0 && bufferGrande[n - 1] == '\n') {
        bufferGrande[n - 1] = '\0';
    } else if (n == sizeof(bufferGrande) - 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }

    strncpy(destino, bufferGrande, (size_t) tamMax - 1);
    destino[tamMax - 1] = '\0';
}

static void inserirManual(FILE *f) {
    Registro r;
    printf("Codigo do segurado (%d caracteres): ", TAM_COD);
    lerLinha(r.codigo, sizeof(r.codigo));
    printf("Nome do segurado (max %d): ", TAM_NOME);
    lerLinha(r.nome, sizeof(r.nome));
    printf("Seguradora (max %d): ", TAM_SEGURADORA);
    lerLinha(r.seguradora, sizeof(r.seguradora));
    printf("Tipo do seguro (max %d): ", TAM_TIPO);
    lerLinha(r.tipo, sizeof(r.tipo));

    int resultado = inserirRegistro(f, &r);
    if (resultado == 1) printf(">> Registro inserido com sucesso.\n");
    else if (resultado == 0) printf(">> ERRO: ja existe um registro com o codigo '%s'.\n", r.codigo);
    else printf(">> ERRO ao inserir registro.\n");
}

static void removerManual(FILE *f) {
    char codigo[TAM_COD + 1];
    printf("Codigo do segurado a remover: ");
    lerLinha(codigo, sizeof(codigo));

    int resultado = removerRegistro(f, codigo);
    if (resultado == 1) printf(">> Registro '%s' removido com sucesso.\n", codigo);
    else printf(">> Codigo '%s' nao encontrado.\n", codigo);
}

static void inserirDaLista(FILE *f, ListaInsercao *lista) {
    if (lista->total == 0) {
        printf(">> Nenhum dado carregado de '%s'. Use a opcao 5 primeiro.\n", ARQ_INSERE);
        return;
    }
    if (lista->indiceAtual >= lista->total) {
        printf(">> Todos os %d registros de '%s' ja foram utilizados.\n", lista->total, ARQ_INSERE);
        return;
    }

    Registro *r = &lista->itens[lista->indiceAtual];
    int resultado = inserirRegistro(f, r);
    if (resultado == 1) {
        printf(">> Inserido registro %d/%d do arquivo carregado: codigo=%s nome=%s\n",
               lista->indiceAtual + 1, lista->total, r->codigo, r->nome);
    } else if (resultado == 0) {
        printf(">> Registro %d/%d (codigo=%s) NAO inserido: codigo ja existe no arquivo de dados.\n",
               lista->indiceAtual + 1, lista->total, r->codigo);
    } else {
        printf(">> ERRO de E/S ao inserir.\n");
    }

    lista->indiceAtual++;
    salvarIndiceSalvo(ARQ_IDX_INSERE, lista->indiceAtual);
}

static void removerDaLista(FILE *f, ListaRemocao *lista) {
    if (lista->total == 0) {
        printf(">> Nenhum dado carregado de '%s'. Use a opcao 5 primeiro.\n", ARQ_REMOVE);
        return;
    }
    if (lista->indiceAtual >= lista->total) {
        printf(">> Todos os %d codigos de '%s' ja foram utilizados.\n", lista->total, ARQ_REMOVE);
        return;
    }

    char *codigo = lista->codigos[lista->indiceAtual];
    int resultado = removerRegistro(f, codigo);
    if (resultado == 1) {
        printf(">> Removido codigo %d/%d: %s\n", lista->indiceAtual + 1, lista->total, codigo);
    } else {
        printf(">> Codigo %d/%d (%s) NAO encontrado no arquivo de dados.\n",
               lista->indiceAtual + 1, lista->total, codigo);
    }

    lista->indiceAtual++;
    salvarIndiceSalvo(ARQ_IDX_REMOVE, lista->indiceAtual);
}

static void carregarArquivosDeTeste(ListaInsercao *listaIns, ListaRemocao *listaRem) {
    liberarListaInsercao(listaIns);
    liberarListaRemocao(listaRem);

    int okIns = carregarInsere(listaIns);
    int okRem = carregarRemove(listaRem);

    if (okIns) printf(">> '%s' carregado: %d registros (%d ja utilizados anteriormente).\n",
                       ARQ_INSERE, listaIns->total, listaIns->indiceAtual);
    else printf(">> AVISO: nao foi possivel abrir '%s'.\n", ARQ_INSERE);

    if (okRem) printf(">> '%s' carregado: %d codigos (%d ja utilizados anteriormente).\n",
                       ARQ_REMOVE, listaRem->total, listaRem->indiceAtual);
    else printf(">> AVISO: nao foi possivel abrir '%s'.\n", ARQ_REMOVE);
}

static void mostrarMenu(void) {
    printf("\n================= CADASTRO GERAL DE SEGURADORAS =================\n");
    printf(" 1 - Insercao   (proximo registro do arquivo carregado insere.bin)\n");
    printf(" 2 - Remocao    (proximo codigo do arquivo carregado remove.bin)\n");
    printf(" 3 - Compactacao\n");
    printf(" 4 - Dump do arquivo\n");
    printf(" 5 - Carrega arquivos (insere.bin e remove.bin)\n");
    printf(" ------------------------------------------------------------------\n");
    printf(" 6 - Insercao manual   (extra, para testar sem os arquivos)\n");
    printf(" 7 - Remocao manual    (extra, para testar sem os arquivos)\n");
    printf(" 0 - Sair\n");
    printf("===================================================================\n");
    printf("Escolha uma opcao: ");
}

int main(void) {
    FILE *f = abrirOuCriarArquivo(ARQ_DADOS);
    if (!f) {
        fprintf(stderr, "Erro fatal: nao foi possivel abrir/criar '%s'.\n", ARQ_DADOS);
        return 1;
    }

    ListaInsercao listaIns = {0};
    ListaRemocao listaRem = {0};

    char linha[16];
    int opcao;
    do {
        mostrarMenu();
        lerLinha(linha, sizeof(linha));
        opcao = atoi(linha);

        switch (opcao) {
            case 1: inserirDaLista(f, &listaIns); break;
            case 2: removerDaLista(f, &listaRem); break;
            case 3: compactarArquivo(&f, ARQ_DADOS); printf(">> Arquivo compactado.\n"); break;
            case 4: dumpArquivo(f); break;
            case 5: carregarArquivosDeTeste(&listaIns, &listaRem); break;
            case 6: inserirManual(f); break;
            case 7: removerManual(f); break;
            case 0: printf("Encerrando...\n"); break;
            default: printf(">> Opcao invalida.\n");
        }
    } while (opcao != 0);

    liberarListaInsercao(&listaIns);
    liberarListaRemocao(&listaRem);
    fclose(f);
    return 0;
}