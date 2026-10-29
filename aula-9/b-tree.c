#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define T 2

// Exercício

typedef struct No {
    int chaves[2 * T - 1];
    struct No *filhos[2 * T];
    int n;
    bool folha;
} No;

typedef struct ArvoreB {
    No *raiz;
} ArvoreB;

No *alocarNo(bool folha) {
    No *novo = malloc(sizeof(No));

    if (novo == NULL) {
        return NULL;
    }

    novo->n = 0;
    novo->folha = folha;

    for (int i = 0; i < 2 * T; i++) {
        novo->filhos[i] = NULL;
    }

    return novo;
}

// Bônus

void inicializar(ArvoreB *arvore) {
    arvore->raiz = NULL;
}

bool buscarNo(No *no, int chave) {
    if (no == NULL) {
        return false;
    }

    int i = 0;

    while (i < no->n && chave > no->chaves[i]) {
        i++;
    }

    if (i < no->n && chave == no->chaves[i]) {
        return true;
    }

    if (no->folha) {
        return false;
    }

    return buscarNo(no->filhos[i], chave);
}

bool buscar(ArvoreB *arvore, int chave) {
    return buscarNo(arvore->raiz, chave);
}

void dividirFilho(No *pai, int i) {
    No *cheio = pai->filhos[i];
    No *novo = alocarNo(cheio->folha);

    if (novo == NULL) {
        fprintf(stderr, "Erro de alocacao.\n");
        exit(EXIT_FAILURE);
    }

    novo->n = T - 1;

    for (int j = 0; j < T - 1; j++) {
        novo->chaves[j] = cheio->chaves[j + T];
    }

    if (!cheio->folha) {
        for (int j = 0; j < T; j++) {
            novo->filhos[j] = cheio->filhos[j + T];
            cheio->filhos[j + T] = NULL;
        }
    }

    int mediana = cheio->chaves[T - 1];
    cheio->n = T - 1;

    for (int j = pai->n; j >= i + 1; j--) {
        pai->filhos[j + 1] = pai->filhos[j];
    }

    pai->filhos[i + 1] = novo;

    for (int j = pai->n - 1; j >= i; j--) {
        pai->chaves[j + 1] = pai->chaves[j];
    }

    pai->chaves[i] = mediana;
    pai->n++;
}

void inserirNaoCheio(No *no, int chave) {
    int i = no->n - 1;

    if (no->folha) {
        while (i >= 0 && chave < no->chaves[i]) {
            no->chaves[i + 1] = no->chaves[i];
            i--;
        }

        no->chaves[i + 1] = chave;
        no->n++;
    } else {
        while (i >= 0 && chave < no->chaves[i]) {
            i--;
        }

        i++;

        if (no->filhos[i]->n == 2 * T - 1) {
            dividirFilho(no, i);

            if (chave > no->chaves[i]) {
                i++;
            }
        }

        inserirNaoCheio(no->filhos[i], chave);
    }
}

void inserir(ArvoreB *arvore, int chave) {
    if (buscar(arvore, chave)) {
        return;
    }

    if (arvore->raiz == NULL) {
        arvore->raiz = alocarNo(true);

        if (arvore->raiz == NULL) {
            fprintf(stderr, "Erro de alocacao.\n");
            exit(EXIT_FAILURE);
        }

        arvore->raiz->chaves[0] = chave;
        arvore->raiz->n = 1;
        return;
    }

    No *raiz = arvore->raiz;

    if (raiz->n == 2 * T - 1) {
        No *novaRaiz = alocarNo(false);

        if (novaRaiz == NULL) {
            fprintf(stderr, "Erro de alocacao.\n");
            exit(EXIT_FAILURE);
        }

        novaRaiz->filhos[0] = raiz;
        dividirFilho(novaRaiz, 0);

        int i = 0;

        if (chave > novaRaiz->chaves[0]) {
            i++;
        }

        inserirNaoCheio(novaRaiz->filhos[i], chave);
        arvore->raiz = novaRaiz;
    } else {
        inserirNaoCheio(raiz, chave);
    }
}

int obterPredecessor(No *no, int idx) {
    No *atual = no->filhos[idx];

    while (!atual->folha) {
        atual = atual->filhos[atual->n];
    }

    return atual->chaves[atual->n - 1];
}

int obterSucessor(No *no, int idx) {
    No *atual = no->filhos[idx + 1];

    while (!atual->folha) {
        atual = atual->filhos[0];
    }

    return atual->chaves[0];
}

void removerDeFolha(No *no, int idx) {
    for (int i = idx + 1; i < no->n; i++) {
        no->chaves[i - 1] = no->chaves[i];
    }

    no->n--;
}

void emprestarAnterior(No *no, int idx) {
    No *filho = no->filhos[idx];
    No *irmao = no->filhos[idx - 1];

    for (int i = filho->n - 1; i >= 0; i--) {
        filho->chaves[i + 1] = filho->chaves[i];
    }

    if (!filho->folha) {
        for (int i = filho->n; i >= 0; i--) {
            filho->filhos[i + 1] = filho->filhos[i];
        }

        filho->filhos[0] = irmao->filhos[irmao->n];
        irmao->filhos[irmao->n] = NULL;
    }

    filho->chaves[0] = no->chaves[idx - 1];
    no->chaves[idx - 1] = irmao->chaves[irmao->n - 1];

    filho->n++;
    irmao->n--;
}

void emprestarProximo(No *no, int idx) {
    No *filho = no->filhos[idx];
    No *irmao = no->filhos[idx + 1];

    filho->chaves[filho->n] = no->chaves[idx];

    if (!filho->folha) {
        filho->filhos[filho->n + 1] = irmao->filhos[0];
    }

    no->chaves[idx] = irmao->chaves[0];

    for (int i = 1; i < irmao->n; i++) {
        irmao->chaves[i - 1] = irmao->chaves[i];
    }

    if (!irmao->folha) {
        for (int i = 1; i <= irmao->n; i++) {
            irmao->filhos[i - 1] = irmao->filhos[i];
        }

        irmao->filhos[irmao->n] = NULL;
    }

    filho->n++;
    irmao->n--;
}

void fundir(No *no, int idx) {
    No *filho = no->filhos[idx];
    No *irmao = no->filhos[idx + 1];

    filho->chaves[T - 1] = no->chaves[idx];

    for (int i = 0; i < irmao->n; i++) {
        filho->chaves[i + T] = irmao->chaves[i];
    }

    if (!filho->folha) {
        for (int i = 0; i <= irmao->n; i++) {
            filho->filhos[i + T] = irmao->filhos[i];
            irmao->filhos[i] = NULL;
        }
    }

    filho->n += irmao->n + 1;

    for (int i = idx + 1; i < no->n; i++) {
        no->chaves[i - 1] = no->chaves[i];
    }

    for (int i = idx + 2; i <= no->n; i++) {
        no->filhos[i - 1] = no->filhos[i];
    }

    no->filhos[no->n] = NULL;
    no->n--;

    free(irmao);
}

void preencherFilho(No *no, int idx) {
    if (idx > 0 && no->filhos[idx - 1]->n >= T) {
        emprestarAnterior(no, idx);
    } else if (idx < no->n &&
               no->filhos[idx + 1]->n >= T) {
        emprestarProximo(no, idx);
    } else {
        if (idx < no->n) {
            fundir(no, idx);
        } else {
            fundir(no, idx - 1);
        }
    }
}

void removerChave(No *no, int chave);

void removerDeInterno(No *no, int idx) {
    int chave = no->chaves[idx];

    if (no->filhos[idx]->n >= T) {
        int pred = obterPredecessor(no, idx);
        no->chaves[idx] = pred;
        removerChave(no->filhos[idx], pred);
    } else if (no->filhos[idx + 1]->n >= T) {
        int suc = obterSucessor(no, idx);
        no->chaves[idx] = suc;
        removerChave(no->filhos[idx + 1], suc);
    } else {
        fundir(no, idx);
        removerChave(no->filhos[idx], chave);
    }
}

void removerChave(No *no, int chave) {
    int idx = 0;

    while (idx < no->n && no->chaves[idx] < chave) {
        idx++;
    }

    if (idx < no->n && no->chaves[idx] == chave) {
        if (no->folha) {
            removerDeFolha(no, idx);
        } else {
            removerDeInterno(no, idx);
        }
        return;
    }

    if (no->folha) {
        return;
    }

    bool ultimo = (idx == no->n);

    if (no->filhos[idx]->n < T) {
        preencherFilho(no, idx);
    }

    if (ultimo && idx > no->n) {
        removerChave(no->filhos[idx - 1], chave);
    } else {
        removerChave(no->filhos[idx], chave);
    }
}

void remover(ArvoreB *arvore, int chave) {
    if (arvore->raiz == NULL || !buscar(arvore, chave)) {
        return;
    }

    removerChave(arvore->raiz, chave);

    if (arvore->raiz->n == 0) {
        No *antigaRaiz = arvore->raiz;

        if (antigaRaiz->folha) {
            arvore->raiz = NULL;
        } else {
            arvore->raiz = antigaRaiz->filhos[0];
            antigaRaiz->filhos[0] = NULL;
        }

        free(antigaRaiz);
    }
}

int main(void) {
    ArvoreB arvore;
    inicializar(&arvore);

    inserir(&arvore, 10);
    inserir(&arvore, 20);
    inserir(&arvore, 5);
    inserir(&arvore, 6);
    inserir(&arvore, 12);
    inserir(&arvore, 30);
    inserir(&arvore, 7);
    inserir(&arvore, 17);

    printf("Buscar 12: %s\n",
           buscar(&arvore, 12) ? "Encontrado" : "Nao encontrado");

    printf("Buscar 99: %s\n",
           buscar(&arvore, 99) ? "Encontrado" : "Nao encontrado");

    remover(&arvore, 6);

    printf("Buscar 6 apos remocao: %s\n",
           buscar(&arvore, 6) ? "Encontrado" : "Nao encontrado");

    return 0;
}