#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <termio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
/**///
#define MAX_THREAD 1000

typedef struct{
    int *retorno; //resultado del factorial 
    int n; //facotrial a calcular
}parm;

void* factorial(void*);

int main(int argc, char *argv[]) {
    int n;
    //cantidad de aegumentos en linea de comandos
    if (argc == 1){
        printf("Usage: %s n\n  where n is no. of fact\n", argv[0]);
        exit(1);
    }
    n = argc-1;
    pthread_t *threads;

    if ((n < 1) || (n > MAX_THREAD)){
        printf("The no of fact should between 1 and %d.\n", MAX_THREAD);
        exit(1);
    }

    //array de elementos a calcular
    parm * elementos = malloc(sizeof(parm)*(n));
    for (int i = 0; i < n; i++){
        (elementos+i)->n=atoi(argv[i+1]); //elemento a calcular
        (elementos+i)->retorno=malloc(sizeof(int)); //donde se guarda el resultado
    }

    //array de hilos para calcular factorial
    threads = (pthread_t *) malloc(n * sizeof(*threads));
    for (int i = 0; i < n; i++){
        pthread_create(&threads[i], NULL, factorial, (void *)(elementos+i));
    }

    //esperar a todos los hilos
    for (int i = 0; i < n; i++){
        pthread_join(threads[i], NULL);
    }

    /* Sincronización completa */
    printf("\nSalida\n");
    for (int i = 0; i < n; i++){
        printf("Factorial de %d: %d\n",(elementos+i)->n, *(elementos+i)->retorno );
    }

    return 0;
}

void *factorial(void *arg){

    parm *p=arg;
    int n=p->n;
    if(n==0){
        *(p->retorno)=1;
        
    }else{
        pthread_t *t_factorial;
        t_factorial = (pthread_t *) malloc(sizeof(pthread_t));
        parm * paramFact = (parm*)malloc(sizeof(parm));
        paramFact->n=n-1;
        paramFact->retorno =(int*) malloc(sizeof(int));
        pthread_create (t_factorial, NULL, factorial, (void *) paramFact);
        pthread_join(*t_factorial, NULL);
        
        *(p->retorno)=n* (*paramFact->retorno);

    }

}