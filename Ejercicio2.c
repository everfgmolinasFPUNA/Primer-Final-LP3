#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>

//para la generación de los números aleatorios
#define MAX_RANDOM 10
//dimension del arreglo de números aleatorios
#define MAX 20

//para definir despues un vector de hilos
typedef struct {
    //donde va a comenzar a buscar
    int inicio;
    //hasta donde puede buscar
    int fin;
    //debe retornar la posicion donde se encontro el elemento
    int *retorno;
}parametro;

int buscar;
int encontrado = 0;
int *vector;

pthread_mutex_t mutex;

//funcion encargada de generar números aleatorios del 0 al 9
int *generar_aleatorios();

//funcion de búsqueda lineal
void *busq_lineal();

//primer argumento: NÚMERO A BUSCAR
//segundo argumento: CANTIDAD DE HILOS A CREAR
int main(int argc, char *argv[]){

    pthread_mutex_init(&mutex,NULL);
    if(argc==1 || argc==2){
        printf("Cantidad de parametros insuficientes\n");
        exit(1);
    } else if(argc>3){
        printf("Cantidad de parametros sobrepasa lo requerido\n");
        exit(1);
    }
    buscar = atoi(argv[1]);
    vector = (int *)malloc(sizeof(int)*MAX);
    vector = generar_aleatorios();
    //cantidad de hilos requeridos
    int cant_hilos = atoi((argv[2]));
    if(cant_hilos > MAX) {
        printf("Dimension del arreglo: %d\n", MAX);
        printf("Cantidad de hilos sobrepasa la dimension del arreglo\n");
        exit(1);
    }
    //cantidad aproximada de celdas por hilo
    int cant_celdas = ceil(MAX/cant_hilos);
    //puntero a hilos para tener tantos hilos como sea necesario
    pthread_t *hilos = (pthread_t *)malloc(sizeof(parametro)*(cant_hilos));
    //cantidad de elementos asociados a cada hilo
    parametro *elementos = (parametro *)malloc(sizeof(parametro)*cant_hilos);
    //asignacion de valores a cada uno de los elementos correspondiente a cada hilo
    for(int i=0; i<cant_hilos; i++){
        if(i==0){
            ((elementos+i)->inicio) = i;
            ((elementos+i)->fin) = cant_celdas-1;
        } else {
            if((elementos+i-1)->fin + cant_celdas < MAX){
                ((elementos+i)->inicio) = ((elementos+(i-1))->fin)+1;
                ((elementos+i)->fin) = ((elementos+(i-1))->fin + cant_celdas);
            } else {
                ((elementos+i)->inicio) = ((elementos+(i-1))->fin)+1;
                ((elementos+i)->fin) = MAX-1;
            }
        }
        ((elementos+i)->retorno) = (int *)malloc(sizeof(int));
        *((elementos+i)->retorno) = -1;
    }
    int nro_hilo = -1;
    //creacion de hilos
    for(int i=0;i<cant_hilos;i++){
        pthread_create((hilos+i),NULL,busq_lineal,(void *)(elementos+i));
    }
    for (int i = 0; i < cant_hilos; i++){
        pthread_join(*(hilos+i), NULL);
    }
    for(int i=0; i<cant_hilos;i++){
        if(*((elementos+i)->retorno) != -1){
            nro_hilo = i;
            break;
        }
    }
    //el vector es
    printf("El vector es: \n");
    for(int i=0; i<MAX;i++){
        printf("vector[%d] = %d\n", i, *(vector + i));
    }
    if(nro_hilo == -1){
        printf("No se ha encontrado el valor %d en el vector\n",buscar);
    } else{
        printf("El numero %d se encuentra en la posicion %d\n",buscar,*((elementos+nro_hilo)->retorno));
    }
    return 0;
}
// funcion encargada de generar los valores aleatorios desde 0 hasta 9
int *generar_aleatorios(){
    
    //puntero a enteros que contendrá los números aleatorios
    int *array_aux = (int *)malloc(sizeof(int)*MAX);

    srand(time(NULL));
    for(int i=0; i<MAX; i++){
        *(array_aux + i) = rand()%MAX_RANDOM;
    }

    return array_aux;

}
//funcion encargada de realizar la búsqueda lineal
void *busq_lineal(void * arg){

    parametro *hilo = arg; 
    //definicion de variables auxiliares
    int ini_aux = hilo->inicio;
    int fin_aux = hilo->fin;
    for(int i=ini_aux; i<=fin_aux;i++){
        if( *(vector + i) == buscar ) {
            pthread_mutex_lock(&mutex);
            *(hilo->retorno) = i;
            encontrado = 1;
            pthread_mutex_unlock(&mutex);
            break;
        }

        pthread_mutex_lock(&mutex);
        if(encontrado==1)
            break;
        pthread_mutex_unlock(&mutex);

    }
    return NULL;
}