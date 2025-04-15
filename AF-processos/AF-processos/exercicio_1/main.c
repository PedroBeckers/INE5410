#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>


//       (pai)      
//         |        
//    +----+----+
//    |         |   
// filho_1   filho_2


// ~~~ printfs  ~~~
// pai (ao criar filho): "Processo pai criou %d\n"
//    pai (ao terminar): "Processo pai finalizado!\n"
//  filhos (ao iniciar): "Processo filho %d criado\n"

// Obs:
// - pai deve esperar pelos filhos antes de terminar!

void ProcessoFilho(int x){
    printf("Processo filho %d criado\n", x);
    fflush(stdout);
}

void ProcessoPai(int x){
    printf("Processo pai criou %d\n", x);
    fflush(stdout);
}


int main(int argc, char** argv) {

    pid_t pid;

    for(int i = 0; i < 2; i++){
        pid = fork();
        if (pid != 0) {
            ProcessoPai(pid);
            wait(NULL);
        }   
        else {
            ProcessoFilho(getpid());
            return 0;
        }    
    }

    printf("Processo pai finalizado!\n"); 

    return 0;

}
