#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

typedef struct cane{
    int * a;
    char * buffer;
}cane;

void* thread_function(void* arg) {
    cane *args = (cane *) arg;
    printf("Thread in esecuzione\n");
    printf("%s-%d\n", args->buffer, *(args->a));
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;
    cane c;
    char * buffer = "il cane";
    int b = 0;
    c.buffer=buffer;
    c.a=&b;
    printf("%s-%d\n", buffer,b);
    buffer="il pupone";
    
    //printf("%s-%d\n", buffer,b);
    // Inizializza gli attributi
    pthread_attr_init(&attr);

    // Imposta lo stato "detached"
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // Imposta una dimensione dello stack personalizzata (1 MB)
    pthread_attr_setstacksize(&attr, 1024 * 1024);

    // Crea il thread con gli attributi
    if (pthread_create(&thread, &attr, thread_function, &c) != 0) {
        perror("Errore nella creazione del thread");
        return 1;
    }
    b=1;
    // Distruggi la struttura degli attributi
    pthread_attr_destroy(&attr);

    printf("Thread creato in modalità detached\n");

    // Fine del programma principale
    pthread_exit(NULL);
}
