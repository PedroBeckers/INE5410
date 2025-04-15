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


// Avalia o resultado no vetor c. Assume-se que todos os ponteiros (a, b, e c)
// tenham tamanho size.
void avaliar(double* a, double* b, double* c, int size);

struct estrutura_thread{
    double *a;
    double *b;
    double *c;
    int idx_inicio;
    int idx_fim;
};

void *funcao(void *arg){
    struct estrutura_thread *estrutura = (struct estrutura_thread *) arg;
    for(int i = estrutura->idx_inicio; i < estrutura->idx_fim; i++){
        estrutura->c[i] = estrutura->a[i] + estrutura->b[i]; 
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    //struct timeval inicio, fim;

    // Gera um resultado diferente a cada execução do programa
    // Se **para fins de teste** quiser gerar sempre o mesmo valor
    // descomente o srand(0)
    srand(time(NULL)); //valores diferentes
    //srand(0);        //sempre mesmo valor

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
    //Cria vetor do resultado 
    double* c = malloc(a_size*sizeof(double));

    // Calcula com uma thread só. Programador original só deixou a leitura 
    // do argumento e fugiu pro caribe. É essa computação que você precisa 
    // paralelizar
    //for (int i = 0; i < a_size; ++i){
    //    c[i] = a[i] + b[i];
    //}

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
        estruturas[i].c = c;
        pthread_create(&threads[i], NULL, funcao, (void *) &estruturas[i]);
        index_inicio_atual = estruturas[i].idx_fim;    
    }

    for(int i = 0; i < n_threads; i++){
        pthread_join(threads[i], NULL);    
    }

    //gettimeofday(&fim, NULL);

    //long segundos = fim.tv_sec - inicio.tv_sec;
    //long microsegundos = fim.tv_usec - inicio.tv_usec;
    //double tempo_gasto = segundos + microsegundos / 1e6;
    //printf("Tempo gasto: %f segundos\n", tempo_gasto);

    //    +---------------------------------+
    // ** | IMPORTANTE: avalia o resultado! | **
    //    +---------------------------------+
    avaliar(a, b, c, a_size);
    
    //Importante: libera memória
    free(a);
    free(b);
    free(c);

    return 0;
}

/*
O speedup obtido está proximo do speedup ideal?
- Para 2 threads o speedup está próximo do ideal, porém para mais threads o speedup fica longe do ideal.
  
   Obtivemos os resultados:
        Número de threads | 2    | 4    | 8    | 10   | 12
        Speedup           | 1,99 | 2,73 | 4,36 | 5,24 | 5,87
  
   Tempos:
        beckerpedro@Becker:~/Documentos/ProgramacaoConcorrente/AF-threads/exercicio_2$ ./program 1 gen:99000000 gen:99000000
        Tempo gasto: 1.678527 segundos
        beckerpedro@Becker:~/Documentos/ProgramacaoConcorrente/AF-threads/exercicio_2$ ./program 2 gen:99000000 gen:99000000
        Tempo gasto: 0.840977 segundos
        beckerpedro@Becker:~/Documentos/ProgramacaoConcorrente/AF-threads/exercicio_2$ ./program 4 gen:99000000 gen:99000000
        Tempo gasto: 0.615869 segundos
        beckerpedro@Becker:~/Documentos/ProgramacaoConcorrente/AF-threads/exercicio_2$ ./program 8 gen:99000000 gen:99000000
        Tempo gasto: 0.385227 segundos
        beckerpedro@Becker:~/Documentos/ProgramacaoConcorrente/AF-threads/exercicio_2$ ./program 10 gen:99000000 gen:99000000
        Tempo gasto: 0.320433 segundos
        beckerpedro@Becker:~/Documentos/ProgramacaoConcorrente/AF-threads/exercicio_2$ ./program 12 gen:99000000 gen:99000000
        Tempo gasto: 0.285829 segundos

O programa escala, ou seja, o speedup aumenta se aumentarmos o número de threads?
- Sabendo que o processador da máquina que executou o código tem 12 núcleos, até 12 threads o programa escala, para mais que
  12 threads o programa começa a ficar mais lento.
  
   Tempo obtido para 13 threads:
        beckerpedro@Becker:~/Documentos/ProgramacaoConcorrente/AF-threads/exercicio_2$ ./program 13 gen:99000000 gen:99000000
        Tempo gasto: 0.326002 segundos
 
*/
