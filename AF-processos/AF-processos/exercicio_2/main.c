#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

//                          (principal)
//                               |
//              +----------------+--------------+
//              |                               |
//           filho_1                         filho_2
//              |                               |
//    +---------+-----------+          +--------+--------+
//    |         |           |          |        |        |
// neto_1_1  neto_1_2  neto_1_3     neto_2_1 neto_2_2 neto_2_3

// ~~~ printfs  ~~~
//      principal (ao finalizar): "Processo principal %d finalizado\n"
// filhos e netos (ao finalizar): "Processo %d finalizado\n"
//    filhos e netos (ao inciar): "Processo %d, filho de %d\n"

// Obs:
// - netos devem esperar 5 segundos antes de imprmir a mensagem de finalizado (e terminar)
// - pais devem esperar pelos seu descendentes diretos antes de terminar

void ProcessoNeto(int x, int y){
    printf("Processo %d, filho de %d\n", x, y);
    fflush(stdout);
    sleep(5);
}

void ProcessoFilho(int x, int y){
    printf("Processo %d, filho de %d\n", x, y);
    fflush(stdout);
}

// n indica profundidade da arvore
void finalizaProcesso(int x, int n){
    if(n == 0){
        printf("Processo principal %d finalizado\n", x);
    }else {
        printf("Processo %d finalizado\n", x);
    }
}

int main(int argc, char** argv) {

    pid_t pid;

    for(int i = 0; i < 2; i++){
        pid = fork();
        if(pid == 0){ //Processos filhos
            ProcessoFilho(getpid(), getppid());
            for(int j = 0; j < 3; j++){
                pid = fork();
                if(pid == 0){ //Processos netos
                    ProcessoNeto(getpid(),getppid());
                    finalizaProcesso(getpid(), 2);
                    return 0;
                }
            }
            wait(NULL);
            finalizaProcesso(getpid(), 1);
            return 0;
        }
    }

    /*************************************************
     * Dicas:                                        *
     * 1. Leia as intruções antes do main().         *
     * 2. Faça os prints exatamente como solicitado. *
     * 3. Espere o término dos filhos                *
     *************************************************/

    while(wait(NULL)>=0);
    finalizaProcesso(getpid(), 0);
    return 0;
}
