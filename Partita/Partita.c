#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../Librerie/Stanze/Stanze.h"
#include "../Librerie/ThreadConnessione/ThreadConnessione.h"
#include "../Librerie/MessageEditor/MessageEditor.h"
#include "../Librerie/GestioneConnessione/GestioneConnessione.h"

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_stato = PTHREAD_MUTEX_INITIALIZER;

int server_fd;
struct sockaddr_in server_addr;
socklen_t addr_len = sizeof(server_addr);
Stanza stanza_corrente;

int AperturaSocket();


void gestioneNuovaConnessione(int * socket, char * buffer, Utente * utente, Message msg);

int main(int argc, char *argv[]){
    

    
    int pipe_read = atoi(argv[1]);
    int pipe_write = atoi(argv[2]);
    read(pipe_read, &stanza_corrente, sizeof(Stanza));
    AperturaSocket();
    printf("porta stanza: %d\n", ntohs(server_addr.sin_port));

    write(pipe_write,&(server_addr.sin_port),sizeof(server_addr.sin_port));
    

    
    // Metti il server in ascolto
    
    if (listen(server_fd, 5) < 0) {
        perror("Errore nella listen");
        close(server_fd);
        return -1;
    }
    
    
    printf("La partita è in ascolto sulla porta %d...\n", ntohs(server_addr.sin_port));

    while (1){
    
        assignConnectionToThread(server_fd,server_addr,addr_len);

    }

    

    close(server_fd);

    return 0;

}


//fare che quando uno chiuda la connessione il thread toglie l'utente
void * Thread_GestioneNuovaConnessione(void *args){
    GestioneConnessioneArgs * arg=(GestioneConnessioneArgs *) args;

    //aggunta handler per rupulire risorse allocate per il thread
    pthread_cleanup_push(cleanup_handler_connection, args);

    printf("%d\n",*(arg->socket));
    read(*(arg->socket), arg->buffer, sizeof(arg->buffer));

    Message msg = dividiStringa(arg->buffer,":",sizeof(arg->buffer));

    initUtente(&(arg->utente),msg.nome,msg.password,msg.lingua,msg.funzione,*(arg->socket));

    //lavoro
    gestioneNuovaConnessione(arg->socket,arg->buffer,&(arg->utente),msg);


    //utilizzo del henadler prima della chiusura
    pthread_cleanup_pop(1);  
    pthread_exit(NULL);
}

 
void addPlayerToParty(Utente * utente){

    //pthread_mutex_lock(&mutex_stato);
    printf("prima culo\n");
    if(getStato(&stanza_corrente, &mutex_stato) == INIZIATA){
        pthread_cond_wait(&cond,&mutex_stato);
    }
    printf("dopo culo\n");
    setNextInOrder(&stanza_corrente, utente);
    printf("dopo set\n");
    
    //pthread_mutex_unlock(&mutex_stato);

}

void broadcast(int * sender_socket, char * sender_messagge){

    pthread_mutex_lock(&(stanza_corrente.light));
    Utente * in_esame = stanza_corrente.listaPartecipanti;
    printf("prima del for\n");
    for(int i=0;i<stanza_corrente.num_players;i++){
        printf("nel for read\n");
        int user_socket = getUserSocket(in_esame);
        if(user_socket != *sender_socket) send(user_socket, sender_messagge, strlen(sender_messagge), 0);
        in_esame = getNext(in_esame);
        
    }
    printf("fine for read\n");
    pthread_mutex_unlock(&(stanza_corrente.light));
}



void chatParty(int * socket, char * buffer, Utente * utente){
    
    char buffer2[BUFFER_SIZE]="";
    strcat(buffer2,utente->nome);
    strcat(buffer2," joined in the chat");
    broadcast(socket,buffer2);
    
    while(getStato(&stanza_corrente, &mutex_stato) == SOSPESA){


        printf("prima read\n");
        printf("%d\n",*(socket));
        riceviRisposta(*socket, buffer, BUFFER_SIZE);
        printf("dopo read\n");

        //aggiusta
        strcpy(buffer2,"");
        strcat(buffer2,"<");
        strcat(buffer2,utente->nome);
        strcat(buffer2,">: ");
        strcat(buffer2,buffer);

        broadcast(socket,buffer2);
        printf("dopo brodcats\n");
    }
    
}

/*
void Game(){

    pthread_mutex_lock(&(stanza_corrente.light));

    Utente * in_esame = stanza_corrente.listaPartecipanti;
    printf("prima del for\n");
    for(int i=0;i<stanza_corrente.num_players;i++){
        printf("nel for read\n");
        int user_socket = getUserSocket(in_esame);
        if(user_socket != *sender_socket) send(user_socket, sender_messagge, strlen(sender_messagge), 0);
        in_esame = getNext(in_esame);
        
    }
    printf("fine for read\n");
    pthread_mutex_unlock(&(stanza_corrente.light));
}*/

void gestioneNuovaConnessione(int * socket, char * buffer, Utente* utente,Message msg){

    printf("prima del add\n");
    printUtente(utente);
    addPlayerToParty(utente);
    printf("dopo del add\n");
    while(getStato(&stanza_corrente, &mutex_stato) != FINITA){
        printf("sto nel  while\n");
        if(getStato(&stanza_corrente, &mutex_stato) == SOSPESA){
            printf("sto nel if del while\n");
            chatParty(socket,buffer,utente);
        }
        pthread_cond_wait(&cond,&mutex_stato);
    }
} 


int AperturaSocket(){   

    // Creazione della socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Impostazione dell'indirizzo della socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = 0;  // Porta 0 per far scegliere al sistema

    // Binding della socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Ottenere la porta assegnata automaticamente
    if (getsockname(server_fd, (struct sockaddr*)&server_addr, &addr_len) == -1) {
        perror("getsockname failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    
    
    return 0;



}


