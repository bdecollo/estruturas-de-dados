#include <stdio.h>

void matching(int lista1[], int n1, int lista2[], int n2, int lista3[], int n3) {
    int i = 0, j = 0, k = 0;

    printf("Elementos comuns: ");

    while (i < n1 && j < n2 && k < n3) {
        if (lista1[i] == lista2[j] && lista2[j] == lista3[k]) {
            printf("%d ", lista1[i]);
            i++;
            j++;
            k++;
        } else if (lista1[i] <= lista2[j] && lista1[i] <= lista3[k]) {
            i++;
        } else if (lista2[j] <= lista1[i] && lista2[j] <= lista3[k]) {
            j++;
        } else k++;
    }
    
    printf("\n");
}

int main() {
    int lista1[] = {10, 25, 30, 45, 60};
    int lista2[] = {10, 25, 30, 40, 50, 70};
    int lista3[] = {1, 8, 10, 18, 25};

    int n1 = 5;
    int n2 = 6;
    int n3 = 5;

    matching(lista1, n1, lista2, n2, lista3, n3);

    return 0;
}