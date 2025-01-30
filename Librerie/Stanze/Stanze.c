#include <string.h>
#include "Stanze.h"

void inserisciStanza(ListStanze* liststanze, Stanza* new){
    
    pthread_mutex_lock(&(liststanze->light));
    new->stato = CREATA;
    if(liststanze->next == NULL){
        new->next=NULL;
        liststanze->next = new;
    }else{
        new->next=liststanze->next;
        liststanze->next=new;
    }
    pthread_mutex_unlock(&(liststanze->light));\
    

}


int existStanza(ListStanze* liststanze, Stanza* check){

    pthread_mutex_lock(&(liststanze->light));
    if(liststanze->next == NULL){
        pthread_mutex_unlock(&(liststanze->light));
        return 0;
    }
    Stanza *tmp = liststanze->next;
    while(tmp != NULL){
        if(strcmp(tmp->nomeStanza,check->nomeStanza) == 0){
            pthread_mutex_unlock(&(liststanze->light));
            return 1;
        }
        tmp = tmp->next;
    }

    pthread_mutex_unlock(&(liststanze->light));
    return 0;
}

ListStanze* initTesta(){
    ListStanze* listStanze = (ListStanze *)malloc(sizeof(ListStanze));
    listStanze->next = NULL; 
    if (pthread_mutex_init(&(listStanze->light), NULL) != 0) {
        perror("Errore durante l'inizializzazione del mutex");
        exit(-1);
    }
    return listStanze;
}

char*  showStanze(ListStanze* liststanze){
    pthread_mutex_lock(&(liststanze->light));
   
    char message[1024];
    char * res = (char *) malloc((1024)*sizeof(char));
 
    Stanza * tmp = liststanze->next;printf("1");
    strcpy(res,"");

    while(tmp != NULL){
        sprintf(message, "%s (%u):",tmp->nomeStanza,tmp->port);
        strcat(res,message);

        tmp = tmp->next;printf("5");
    }


    

    pthread_mutex_unlock(&(liststanze->light));
    return res;

}


int removeStanza(ListStanze* liststanze, char* nomeStanza){
    
    pthread_mutex_lock(&(liststanze->light));
    if(liststanze->next == NULL){
        pthread_mutex_unlock(&(liststanze->light));
        return 0;
    }

    Stanza *tmp = liststanze->next;
    Stanza *pre = NULL;
    
    while(tmp != NULL){
        if(pre == NULL && strcmp(tmp->nomeStanza,nomeStanza)==0){
            liststanze->next = tmp->next;
            free(tmp);
            pthread_mutex_unlock(&(liststanze->light));
            return 1;
        }else if(strcmp(tmp->nomeStanza,nomeStanza)==0){
            pre->next=tmp->next;
            free(tmp);
            pthread_mutex_unlock(&(liststanze->light));
            return 1;
        }
        pre = tmp;
        tmp=tmp->next;
    }
    pthread_mutex_unlock(&(liststanze->light));
    return 0;

}

ListStanze* freeStanze(ListStanze* liststanze){
    if(liststanze->next == NULL){
        return NULL;
    }
    Stanza *tmp = liststanze->next;
    Stanza *pre = NULL;
    while(tmp != NULL){
        pre = tmp;
        tmp = tmp->next;
        free(pre);
    }
    pthread_mutex_destroy(&(liststanze->light));
    free(liststanze);
    return NULL;
}