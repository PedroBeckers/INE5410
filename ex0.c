#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct MinMax
{
    int min;
    int max;
}MinMax;

MinMax *getMinMax(int * array, const int SIZE) {

    MinMax *ans = (MinMax *) malloc(sizeof(MinMax));

    ans->min = array[0];
    ans->max = array[0];

    for (int i = 1; i < SIZE; i++) {
        if (array[i] < ans->min) {
            ans->min = array[i];
        }
        if (array[i] > ans->max) {
            ans->max = array[i];
        }
    }

    return ans;

}

//Lê os elementos do teclado e os coloca em arr. Size possui o tamanho do vetor
void inputArray(int * arr, int size) {
    for(int i = 0; i < size; i++) {
        scanf("%d", &arr[i]);
    }
}

//Imprime o conteúdo do vetor arr com tamanho size
void printArray(int * arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
}

//Função que compara dois inteiros. A função retorna a diferença entre o primeiro e o segundo
int sortAscending(int * num1, int * num2) {
    return *num1 - *num2;
}

//Função que compara dois inteiros. A função retorna a diferença entre o segundo e o primeiro
int sortDescending(int * num1, int * num2) {
    return *num2 - *num1;
}

void swap(int * num1, int * num2) {
    int temp = *num1;
    *num1 = *num2;
    *num2 = temp;
}

/*
Função que realizada a ordenação. O terceiro argumento é um ponteiro para função que
realiza a comparação entre dois inteiros do vetor (funções sortAscending ou sortDescending)
*/
void sort(int * arr, int size, int (* compare)(int *, int *)) {

    bool swapped;

    for (int i = 0; i < size; i++) {
        swapped = false;
        for (int j = 0; j < size - i - 1; j++) {
            if (compare(&arr[j], &arr[j + 1]) > 0) {
                swapped = true;
                swap(&arr[j], &arr[j + 1]);
            }
        }

        if(!swapped){
            break;
        }

    }
}


int main() {

    int arr[10]; //alterar max_size
    int size;

    /*
    * Input array size and elements.
    */
    printf("Enter array size: ");
    scanf("%d", &size);
    printf("Enter elements in array: ");
    inputArray(arr, size);
    printf("\nElements before sorting: ");
    printArray(arr, size);

    // Sort and print sorted array in ascending order.`
    printf("\n\nArray in ascending order: ");
    sort(arr, size, sortAscending);
    printArray(arr, size);

    // Sort and print sorted array in descending order.
    printf("\nArray in descending order: ");
    sort(arr, size, sortDescending);
    printArray(arr, size);

    return 0;
}

