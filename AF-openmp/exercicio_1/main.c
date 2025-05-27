#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>

void calcular(double* c, int size, int n_threads) {
    #pragma omp parallel for 
    for (long long int i = 0; i < size; i++) {
        c[i] = sqrt(i * 32) + sqrt(i * 16 + i * 8) + sqrt(i * 4 + i * 2 + i);
        c[i] -= sqrt(i * 32 * i * 16 + i * 4 + i * 2 + i);
        c[i] += pow(i * 32, 8) + pow(i * 16, 12);
    }
}


int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Uso: %s threads [tamanho]\n", argv[0]);
        return 1;
    }
    int n_threads = atoi(argv[1]);
    int size = argc > 2 ? atoi(argv[2]) : 20000000;

    double *c = (double *) malloc (sizeof(double) * size);

    omp_set_num_threads(n_threads);
    
    //Guarda ponto de início da computação
    double start = omp_get_wtime();
    calcular(c, size, n_threads);
    double duration = omp_get_wtime()-start; //quanto tempo passou
    printf("n_threads: %d, size: %d, tempo: %.3f secs\n", 
           n_threads, size, duration);

    free(c);

    return 0;
}

/*

$ ./program 1 100000000
n_threads: 1, size: 100000000, tempo: 3.180 secs

$ ./program 2 100000000
n_threads: 2, size: 100000000, tempo: 1.614 secs

$ ./program 4 100000000
n_threads: 4, size: 100000000, tempo: 1.274 secs

$ ./program 6 100000000
n_threads: 6, size: 100000000, tempo: 1.071 secs

$ ./program 8 100000000
n_threads: 8, size: 100000000, tempo: 0.896 secs

$ ./program 10 100000000
n_threads: 10, size: 100000000, tempo: 0.774 secs

$ ./program 12 100000000
n_threads: 12, size: 100000000, tempo: 0.719 secs

|-----------------------|
| Threads  |   Speedup  |
|-----------------------|
|     1    |    1.00    |
|     2	   |    1.97    |
|     4	   |    2.50    |
|     6	   |    3.55    |
|     8	   |    4.11    |
|     10   |    4.42    |
|     12   |    4.42    | 
|-----------------------|


*/

