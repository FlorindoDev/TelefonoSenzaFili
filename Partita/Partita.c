#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../Librerie/Stanze/Stanze.h"
//#include "../Librerie/ThreadConnessione/ThreadConnessione.h"


int server_fd;
struct sockaddr_in server_addr;
socklen_t addr_len = sizeof(server_addr);
Stanza stanza_corrente;



/*

char* riceviRisposta(){

    int charPassati = read(sock, buffer, BUFFER_SIZE);
        if (charPassati < BUFFER_SIZE){
            buffer[charPassati]='\0';
        }
    printf("Risposta dal server: %s\n", buffer);
    return buffer;
}

void gestioneNuovaConnessione(int socket, char * buffer, Utente * utente);



void * Thread_GestioneNuovaConnessione(void *args){
    GestioneConnesioneArgs * arg=(GestioneConnesioneArgs *) args;

    //aggunta handler per rupulire risorse allocate per il thread
    pthread_cleanup_push(cleanup_handler_connection, args);
    
    //lavoro
    gestioneNuovaConnessione(arg->socket,arg->buffer,&(arg->utente));


    //utilizzo del henadler prima della chiusura
    pthread_cleanup_pop(1);  
    pthread_exit(NULL);
}


void addPlayerToParty(Utente * utente){



}

void broadcast(int * sender_socket, char * sender_messagge){

    pthread_mutex_lock(&(stanza_corrente->light));
    Utente * in_esame = stanza_corrente->listaPartecipanti;
    for(int i=0;i<stanza_corrente->num_players;i++){
        in_esame = getNext(in_esame);
        int user_socket = getUserSocket(in_esame);
        if(user_socket != *sender_socket) send(user_socket, sender_messagge, strlen(sender_messagge), 0);
        
    }
    pthread_mutex_unlock(&(stanza_corrente->light));
}

Stato getStato(){
    pthread_mutex_lock(&(stanza_corrente->light));
    Stato tmp = stanza_corrente.stato;
    pthread_mutex_unlock(&(stanza_corrente->light));
    return tmp;
}

void chatParty(int * socket, char * buffer, Utente * utente){
    
    while(getStato() == SOSPESA){
        riceviRisposta(*socket, buffer, BUFFER_SIZE);
        broadcast(socket,buffer);
    }
    

}

void gestioneNuovaConnessione(int socket, char * buffer, Utente * utente){

    if(getStato() == SOSPESA){
        addPlayerToParty();
        chatParty();
        Game();

    }
    

}
*/

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


int main(int argc, char *argv[]){
    

    
    int pipe_read = atoi(argv[1]);
    int pipe_write = atoi(argv[2]);
    read(pipe_read, &stanza_corrente, sizeof(Stanza));
    
    AperturaSocket();
    printf("porta stanza: %d\n", ntohs(server_addr.sin_port));

    write(pipe_write,&(server_addr.sin_port),sizeof(server_addr.sin_port));
    

    /*
    // Metti il server in ascolto
    if (listen(server_fd, 5) < 0) {
        perror("Errore nella listen");
        close(server_fd);
        return -1;
    }
    
    printf("Server in ascolto sulla porta %d...\n", ntohs(server_addr_stanza.sin_port));

    while (1){
    
        
        assignConnectionToThread(server_fd,server_addr,server_addr);
        

    }

    */


    close(server_fd);
    return 0;
}


