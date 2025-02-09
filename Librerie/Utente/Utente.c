#include <string.h>


void initUtente(Utente * utente,char nome[MAX_UTENTE], char password[MAX_UTENTE] ,char lingua[MAX_UTENTE], char funzione[MAX_UTENTE], int socket){

    strcpy(utente->nome,nome);
    strcpy(utente->password,password);
    strcpy(utente->lingua,lingua);
    strcpy(utente->funzione,funzione);
    utente->socket=socket;
    utente->next=NULL;
    utente->prev=NULL;
}

int getUserSocket(Utente * utente){
    return utente->socket;
}

void printUtente(Utente * utente){
    if(utente == NULL) return;
    printf("\n-----\n");
    printf("Nome:%s\nPassowrd:%s\nLingua:%s\nFunzione:%s\nSocket:%d\nProssimo:%p\nPrev:%p\n", utente->nome,utente->password,utente->lingua,utente->funzione,utente->socket,utente->next,utente->prev);
    printf("\n-----\n");
}

Utente * getNext(Utente * utente){
    if(utente != NULL)return utente->next;
}


void setPthread(Utente * utente , pthread_t tid){
    utente->thread=tid;
}

int removeUtenteFromThread(Utente * utente, pthread_t thread, int num_players ){

    Utente * in_esame = utente;
    for(int i=0;i<num_players;i++){
        if(in_esame->thread==thread){
            if(in_esame->next != NULL){
                in_esame->next->prev=in_esame->prev;
            }else{
                if(in_esame->prev != NULL){
                    in_esame->prev->next=NULL;
                }
            }
            if(in_esame->prev != NULL){
                in_esame->prev->next=in_esame->next;
            }else{
                if(in_esame->next != NULL){
                    in_esame->next->prev=NULL;
                }
            }
            free(in_esame);
            num_players--;
        }
        in_esame = getNext(in_esame);
    }
    
    return (num_players==0) ? -1 : 1;
    
}

