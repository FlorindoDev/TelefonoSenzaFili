#include <string.h>


void initUtente(Utente * utente,char nome[MAX_UTENTE], char password[MAX_UTENTE] ,char lingua[MAX_UTENTE], char funzione[MAX_UTENTE]){

    strcpy(utente->nome,nome);
    strcpy(utente->password,password);
    strcpy(utente->lingua,lingua);
    strcpy(utente->funzione,funzione);
    utente->next=NULL;
    utente->prev=NULL;
}

int getUserSocket(Utente * utente){
    return utente->socket;
}
