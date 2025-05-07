#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

// Lê o conteúdo do arquivo filename e retorna um vetor E o tamanho dele
// Se filename for da forma "gen:%d", gera um vetor aleatório com %d elementos
//
// +-------> retorno da função, ponteiro para vetor malloc()ado e preenchido
// | 
// |         tamanho do vetor (usado <-----+
// |         como 2o retorno)              |
// v                                       v
double* load_vector(const char* filename, int* out_size);


// Avalia se o prod_escalar é o produto escalar dos vetores a e b. Assume-se
// que ambos a e b sejam vetores de tamanho size.
void avaliar(double* a, double* b, int size, double prod_escalar);

struct estrutura_thread{
    double *a;
    double *b;
    int idx_inicio;
    int idx_fim;
};

void *funcao(void *arg){
    struct estrutura_thread *estrutura = (struct estrutura_thread *) arg;
    double *soma = malloc(sizeof(double));
    for(int i = estrutura->idx_inicio; i < estrutura->idx_fim; i++){
        *soma += estrutura->a[i] * estrutura->b[i]; 
    }
    pthread_exit(soma);
}

int main(int argc, char* argv[]) {
    //struct timeval inicio, fim;

    srand(time(NULL));
    srand(0);

    //Temos argumentos suficientes?
    if(argc < 4) {
        printf("Uso: %s n_threads a_file b_file\n"
               "    n_threads    número de threads a serem usadas na computação\n"
               "    *_file       caminho de arquivo ou uma expressão com a forma gen:N,\n"
               "                 representando um vetor aleatório de tamanho N\n", 
               argv[0]);
        return 1;
    }
  
    //Quantas threads?
    int n_threads = atoi(argv[1]);
    if (!n_threads) {
        printf("Número de threads deve ser > 0\n");
        return 1;
    }
    //Lê números de arquivos para vetores alocados com malloc
    int a_size = 0, b_size = 0;
    double* a = load_vector(argv[2], &a_size);
    if (!a) {
        //load_vector não conseguiu abrir o arquivo
        printf("Erro ao ler arquivo %s\n", argv[2]);
        return 1;
    }
    double* b = load_vector(argv[3], &b_size);
    if (!b) {
        printf("Erro ao ler arquivo %s\n", argv[3]);
        return 1;
    }
    
    //Garante que entradas são compatíveis
    if (a_size != b_size) {
        printf("Vetores a e b tem tamanhos diferentes! (%d != %d)\n", a_size, b_size);
        return 1;
    }

    //Calcula produto escalar. Paralelize essa parte
    //double result = 0;
    //for (int i = 0; i < a_size; ++i) 
    //    result += a[i] * b[i];
    

    // Numero de threads nao pode ser maior que o numero de elementos do array
    n_threads = n_threads <= a_size ? n_threads : a_size;

    pthread_t threads[n_threads];
    struct estrutura_thread estruturas[n_threads];

    int elementos_por_thread = a_size/n_threads; 
    int elementos_restantes = a_size % n_threads;

    // Construindo estruturas de todas as threads da lista e criando as threads
    int index_inicio_atual = 0; 
    int index_fim_atual;

    // Medindo tempo apenas da fracao paralelizavel do codigo
    //gettimeofday(&inicio, NULL);

    for(int i = 0; i < n_threads; i++){

        // Caso a_size/n_threads nao seja uma divisao exata
        if (elementos_restantes) {
            index_fim_atual = index_inicio_atual + elementos_por_thread + 1;
            elementos_restantes--;
        } else {
            index_fim_atual = index_inicio_atual + elementos_por_thread;
        }

        estruturas[i].idx_inicio = index_inicio_atual;
        estruturas[i].idx_fim = index_fim_atual;
        estruturas[i].a = a;
        estruturas[i].b = b;
        pthread_create(&threads[i], NULL, funcao, (void *) &estruturas[i]);
        index_inicio_atual = estruturas[i].idx_fim;    
    }

    double result = 0;
    for(int i = 0; i < n_threads; i++){
        double *valor_retornado;
        pthread_join(threads[i], (void **) &valor_retornado);
        
        result += *valor_retornado;
        free(valor_retornado);
    }

    //gettimeofday(&fim, NULL);
    //long segundos = fim.tv_sec - inicio.tv_sec;
    //long microsegundos = fim.tv_usec - inicio.tv_usec;
    //double tempo_gasto = segundos + microsegundos / 1e6;
    //printf("Tempo gasto: %f segundos\n", tempo_gasto);

    //    +---------------------------------+
    // ** | IMPORTANTE: avalia o resultado! | **
    //    +---------------------------------+
    avaliar(a, b, a_size, result); //deve ser comentado para avaliacao de speedup

    //Libera memória
    free(a);
    free(b);

    return 0;
}