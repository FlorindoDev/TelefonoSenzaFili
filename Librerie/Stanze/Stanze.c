#include <string.h>
#include "Stanze.h"

void inserisciStanza(ListStanze* liststanze, Stanza* new){
    if(liststanze->next == NULL){
        new->next=NULL;
        liststanze->next = new;
    }else{
        new->next=liststanze->next;
        liststanze->next=new;
    }


}


int existStanza(ListStanze* liststanze, Stanza* check){

    if(liststanze->next == NULL) return 0;
    Stanza *tmp = liststanze->next;
    while(tmp != NULL){
        if(strcmp(tmp->nomeStanza,check->nomeStanza) == 0)return 1;
        tmp = tmp->next;
    }

    return 0;
}

ListStanze* initTesta(){
    ListStanze* listStanze = (ListStanze *)malloc(sizeof(ListStanze));
    listStanze->next = NULL; 
    if (pthread_mutex_init(&listStanze->light, NULL) != 0) {
        perror("Errore durante l'inizializzazione del mutex");
        exit(-1);
    }
    return listStanze;
}