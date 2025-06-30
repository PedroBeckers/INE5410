#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

/*

Cada thread esta encapsulada em uma estrutura "thread_t".
Essa estrutura possui 3 campos: 
    1) id      -> Identificação da thread encapsulada
    2) thread  -> Thread em questão
    3) tarefas -> Ponteiro para a lista de tarefas

Desse modo, consigo consigo encapsular a estrutura tarefa dentro da estrutura thread. Então, consigo passar esses 2 parâmetros unificados
como argumento para o método thread_executa. Por exemplo, cada thread receberá como argumento uma estrutura thread_t, que engloba a lista
de tarefas.

*/

// Declaração de estruturas e variáveis
typedef struct {
    unsigned int intervalo;         // Tamanho do intervalo
    unsigned int indice_inicio;     // Índice de início do intervalo da task | 
    unsigned int indice_fim;        // Índice de fim do intervalo da task    | -> [a, b)
    unsigned int id;                // Identificador da tarefa
    unsigned int tam_vetor_parcial; // Quantidade de elementos em vetor_parcial
    unsigned int *vetor_parcial;    // Ponteiro para o vetor parcial ordenado
} tarefa;

typedef struct {
    unsigned int id;                  // Identificador da thread
    pthread_t thread;                 // Declaração da thread
    tarefa* tarefas;                  // Ponteiro para lista de tarefas
    unsigned int qtd_tarefas;         // Quantidade total de tarefas
    unsigned int *indice_prox_tarefa; // Ponteiro que indica índice da lista de tarefas que contém a próxima tarefa
} thread_t;

pthread_mutex_t mutex;   // Protege o acesso a variável compartilhada indice_prox_tarefa

// Funcao de ordenacao fornecida. Não pode alterar.
void bubble_sort(int *v, int tam){
    int i, j, temp, trocou;

    for(j = 0; j < tam - 1; j++){
        trocou = 0;
        for(i = 0; i < tam - 1; i++){
            if(v[i + 1] < v[i]){
                temp = v[i];
                v[i] = v[i + 1];
                v[i + 1] = temp;
                trocou = 1;
            }
        }
        if(!trocou) break;
    }
}

// Funcao para imprimir um vetor. Não pode alterar.
void imprime_vet(unsigned int *v, int tam) {
    int i;
    for(i = 0; i < tam; i++)
        printf("%d ", v[i]);
    printf("\n");
}

// Funcao para ler os dados de um arquivo e armazenar em um vetor em memoria. Não pode alterar.
int le_vet(char *nome_arquivo, unsigned int *v, int tam) {
    FILE *arquivo;
    
    // Abre o arquivo
    arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    // Lê os números
    for (int i = 0; i < tam; i++)
        fscanf(arquivo, "%u", &v[i]);

    fclose(arquivo);

    return 1;

}

void * thread_executa(void *arg){
    thread_t *t = (thread_t *) arg;

    tarefa tarefa_atual;

    while(1){

        pthread_mutex_lock(&mutex);

        if(*(t->indice_prox_tarefa) >= t->qtd_tarefas){ 
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }

        tarefa_atual = t->tarefas[*t->indice_prox_tarefa];
        (*t->indice_prox_tarefa)++;

        //pthread_mutex_unlock(&mutex); --> desse modo o desempenho é mais eficiente, porém as tarefas nao sao printadas em ordem
 
        printf("Thread %d processando tarefa %d\n", t->id, tarefa_atual.id);
        fflush(stdout);

        pthread_mutex_unlock(&mutex); // --> desse modo o desempenho é menos eficiente, porém as tarefas sao printadas em ordem

        bubble_sort((int *)tarefa_atual.vetor_parcial, tarefa_atual.tam_vetor_parcial);
    }
}

// Funcao principal de ordenacao. Deve ser implementada com base nas informacoes fornecidas no enunciado do trabalho.
// Os numeros ordenados deverao ser armazenanos no proprio "vetor".
int sort_paralelo(unsigned int *vetor, unsigned int nnumbers, unsigned int ntasks, unsigned int nthreads) {

    // Restrições
    if(nthreads < 1 || ntasks > nnumbers){
        printf("Parâmetros inválidos");
        return 0;
    }

    // Etapa 1 ---> Criação das tarefas
    tarefa *tarefas = malloc(ntasks*sizeof(tarefa));

    // Tratamento do caso onde nnumbers não é multiplo de ntasks
    unsigned int intervalo_resto = nnumbers % ntasks;
    unsigned int intervalo_inteiro = nnumbers / ntasks;

    // Inicializando os membros das estruturas tarefa da lista tarefas
    unsigned int ultimo_indice_fim = 0;
    for(unsigned int i = 0; i < ntasks; i++){

        if(intervalo_resto){
            tarefas[i].intervalo = intervalo_inteiro + 1;
            intervalo_resto--;
        }else{
            tarefas[i].intervalo = intervalo_inteiro;
        }

        tarefas[i].indice_inicio = ultimo_indice_fim;
        tarefas[i].indice_fim = tarefas[i].indice_inicio + tarefas[i].intervalo;
        tarefas[i].id = i;

        unsigned int tamanho_vetor_parcial = 0;     //TRATAR CASO ONDE ESSA VARIAVEL VALE 0
        for(unsigned int j = 0; j < nnumbers; j++){
            if(vetor[j] >= tarefas[i].indice_inicio && vetor[j] < tarefas[i].indice_fim){
                tamanho_vetor_parcial++;
            }
        }

        tarefas[i].tam_vetor_parcial = tamanho_vetor_parcial;
        tarefas[i].vetor_parcial = malloc(tamanho_vetor_parcial*sizeof(unsigned int));

        unsigned int indice_vetor_parcial = 0;
        for(unsigned int j = 0; j < nnumbers; j++){
            if(vetor[j] >= tarefas[i].indice_inicio && vetor[j] < tarefas[i].indice_fim){
                tarefas[i].vetor_parcial[indice_vetor_parcial] = vetor[j];
                indice_vetor_parcial++;
            }
        }

        ultimo_indice_fim = tarefas[i].indice_fim;
    }

    // Etapa 2 ---> Criação das threads
    pthread_mutex_init(&mutex, NULL);
    thread_t *threads = malloc(nthreads*sizeof(thread_t));
    unsigned int *indice = malloc(sizeof(unsigned int));
    *indice = 0;
    
    // Inicializando os membros das estruturas thread_t da lista threads
    for(unsigned int i = 0; i < nthreads; i++){
        threads[i].id = i;
        threads[i].tarefas = tarefas;
        threads[i].qtd_tarefas = ntasks;
        threads[i].indice_prox_tarefa = indice;
    }

    // Criando as threads
    for(unsigned int i = 0; i < nthreads; i++){
        pthread_create(&threads[i].thread, NULL, thread_executa, (void *) &threads[i]);
    }

    // Etapa 3 -> Concatenação dos resultados de cada thread
    for(unsigned int i = 0; i < nthreads; i++){
        pthread_join(threads[i].thread, NULL);
    }

    unsigned int iterador_vetor = 0;
    for(unsigned int i = 0; i < ntasks; i++){
        for(unsigned int j = 0; j < tarefas[i].tam_vetor_parcial; j++){
            vetor[iterador_vetor] = tarefas[i].vetor_parcial[j];
            iterador_vetor++;
        }
    }

    // Etapa 4 -> Finalizar alocações dinâmicas
    free(threads);
    free(indice);
    for(unsigned int i = 0; i < ntasks; i++){
        free(tarefas[i].vetor_parcial);
    }
    free(tarefas);
    pthread_mutex_destroy(&mutex);

    return 1;
}

// Funcao principal do programa. Não pode alterar.
int main(int argc, char **argv) {
    
    // Verifica argumentos de entrada
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <input> <nnumbers> <ntasks> <nthreads>\n", argv[0]);
        return 1;
    }

    // Argumentos de entrada
    unsigned int nnumbers = atoi(argv[2]);
    unsigned int ntasks = atoi(argv[3]);
    unsigned int nthreads = atoi(argv[4]);
    
    // Aloca vetor
    unsigned int *vetor = malloc(nnumbers * sizeof(unsigned int));

    // Variaveis de controle de tempo de ordenacao
    struct timeval inicio, fim;

    // Le os numeros do arquivo de entrada
    if (le_vet(argv[1], vetor, nnumbers) == 0)
        return 1;

    // Imprime vetor desordenado
    imprime_vet(vetor, nnumbers);

    // Ordena os numeros considerando ntasks e nthreads
    gettimeofday(&inicio, NULL);
    sort_paralelo(vetor, nnumbers, ntasks, nthreads);
    gettimeofday(&fim, NULL);

    // Imprime vetor ordenado
    imprime_vet(vetor, nnumbers);

    // Desaloca vetor
    free(vetor);

    // Imprime o tempo de ordenacao
    printf("Tempo: %.6f segundos\n", fim.tv_sec - inicio.tv_sec + (fim.tv_usec - inicio.tv_usec) / 1e6);
    
    return 0;
}