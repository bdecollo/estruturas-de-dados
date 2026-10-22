#include <stdio.h>

void merging(int lista1[], int n1, int lista2[], int n2, int lista3[]) {
    int i = 0, j = 0, k = 0;

    while (i < n1 && j < n2) {
        if (lista1[i] <= lista2[j]) {
            lista3[k] = lista1[i];
            i++;
        } else {
            lista3[k] = lista2[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        lista3[k] = lista1[i];
        i++;
        k++;
    }

    while (j < n2) {
        lista3[k] = lista2[j];
        j++;
        k++;
    }
}

int main() {
    int lista1[] = {10, 25, 30, 45, 60};
    int lista2[] = {5, 20, 30, 40, 50, 70};
    int lista3[11];

    int n1 = 5, n2 = 6;
    int i;

    merging(lista1, n1, lista2, n2, lista3);

    printf("Lista resultante: ");
    for (i = 0; i < n1 + n2; i++) {
        printf("%d ", lista3[i]);
    }

    printf("\n");

    return 0;
}