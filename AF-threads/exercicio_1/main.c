#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

//                 (main)      
//                    |
//    +----------+----+------------+
//    |          |                 |   
// worker_1   worker_2   ....   worker_n


// ~~~ argumentos (argc, argv) ~~~
// ./program n_threads

// ~~~ printfs  ~~~
// main thread (após término das threads filhas): "Contador: %d\n"
// main thread (após término das threads filhas): "Esperado: %d\n"

// Obs:
// - pai deve criar n_threds (argv[1]) worker threads 
// - cada thread deve incrementar contador_global (operador ++) n_loops vezes
// - pai deve esperar pelas worker threads  antes de imprimir!


int contador_global = 0;

void *funcao(void *arg){
    int  *n_loops = (int *) arg;
    for(int i = 0; i < *n_loops; i++){
        contador_global++;
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("n_threads é obrigatório!\n");
        printf("Uso: %s n_threads n_loops\n", argv[0]);
        return 1;
    }

    int n_threads = atoi(argv[1]);
    int n_loops = atoi(argv[2]);

    pthread_t thread[n_threads];
    for(int i = 0; i < n_threads; i++){
        pthread_create(&thread[i], NULL, funcao, (void *) &n_loops);    
    }
    for(int i = 0; i < n_threads; i++){
        pthread_join(thread[i], NULL);    
    }
    
    printf("Contador: %d\n", contador_global);
    printf("Esperado: %d\n", n_threads*n_loops);
    return 0;
}


/*Qual a relação entre n_threads e a diferença entre "Contador" e "Esperado"?
    quanto maior n_threads, maior diferença entre contador e esperado
*/

/*Qual a relação entre n_loops e a diferença entre "Contador" e "Esperado"?
    quanto maior n_loops, maior diferença entre contador e esperado
*/

/*Existe alguma regra ou padrão na diferença entre "Contador" e "Esperado"?
    para baixos valores de n_loop, já existe uma pequena diferenca entre contador e esperado a partir de n_threads = 10.
    para altos valores de n_loop, já existe grande diferença entre contador e esperado a partir de n_threads = 2.
    porém o tamanho absoluto da diferença não segue padrão.
*/

