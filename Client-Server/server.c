#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


#include "../Librerie/Autenticazione/auth.h"
#include "../Librerie/connessioneDB/connectionDB.h"
#include "../Librerie/Stanze/Stanze.h"
#include "../Librerie/Utente/Utente.h"
#include "../Librerie/ThreadConnessione/ThreadConnessione.h"
#include "../Librerie/MessageEditor/MessageEditor.h"


//il BUFFER_SIZE STA IN ThreadConnessione
#define PORT 8080
 

ListStanze* listStanze = NULL;


char* controlloRichiestaUtente(const char *, Utente *);
void registerUser(PGconn * , char *, Utente *);
void loginUser(PGconn * , char *, Utente *);
void gestioneNuovaConnessione(int* new_socket, char* buffer, Utente * utente);
pid_t creazioneProcessoStanza(int* fd);
int returnPortaPartita(int* fd);


int mainServer() {

    //inzializza la lista delle stanze
    listStanze=initTesta();


    int server_fd;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    
    // Creazione della socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Errore nella creazione della socket");
        return -1;
    }

    // Configurazione dell'indirizzo del server
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind della socket all'indirizzo e alla porta
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Errore nel binding");
        close(server_fd);
        return -1;
    }

    // Metti il server in ascolto
    if (listen(server_fd, 3) < 0) {
        perror("Errore nella listen");
        close(server_fd);
        return -1;
    }
    
    printf("Server in ascolto sulla porta %d...\n", PORT);

    while (1){

        assignConnectionToThread(server_fd,address,addrlen);
        
    }
    
    
    close(server_fd);

    return 0;
}


//la definzione sta in `ThreadConnessione.h`
void * Thread_GestioneNuovaConnessione(void *args){
    GestioneConnessioneArgs * arg=(GestioneConnessioneArgs *) args;

    //aggunta handler per rupulire risorse allocate per il thread
    pthread_cleanup_push(cleanup_handler_connection, args);
    
    //lavoro
    gestioneNuovaConnessione(arg->socket,arg->buffer,&(arg->utente));
    
    printStanze(listStanze);

    //utilizzo del henadler prima della chiusura
    pthread_cleanup_pop(1);  
    pthread_exit(NULL);
}


void gestioneNuovaConnessione(int * new_socket, char* buffer, Utente * utente){

    // Riceve il messaggio dal client
    read(*new_socket, buffer, BUFFER_SIZE);
    printf("\nElaborazione della stringa ricevuta...\n");
    printf("\nRicevuto %s, sono %lu\n", buffer, pthread_self());
    // Gestisce la stringa separata da ":"
    const char *response;
        
    //mi faccio dare la risposta e gli do l'utente per riempirlo
    response = controlloRichiestaUtente(buffer, utente);
    
    printf("MEssaggio da inviare %s\n", response);
    send(*new_socket, response, strlen(response), 0);
    // Chiudi la connessione
    close(*new_socket);

}

void loginUser(PGconn* conn, char * response, Utente * utente){
    conn = connect_to_DB();
    response = login(conn,utente) ? "1" : "-1";
    PQfinish(conn);
    
}

void registerUser(PGconn* conn, char * response, Utente * utente){
    conn = connect_to_DB();
    response = register_user(conn,utente) ? "1" : "-1";
    PQfinish(conn);
    
}

char* controlloRichiestaUtente(const char *input, Utente * utente) {

    char* response = "";
    
    Message msg = dividiStringa(input, ":", BUFFER_SIZE);
    
    PGconn* conn = NULL;

    initUtente(utente,msg.nome,msg.password,msg.lingua,msg.funzione);

    if(isSingUp(&msg)){
        
        registerUser(conn, response, utente);


    }else if (isLogin(&msg)){
        
        loginUser(conn, response, utente);
        

    }else if (isCreate(&msg)){
        
        Stanza * tmp = (Stanza *) malloc(sizeof(Stanza));
        Utente * proprietario = (Utente *) malloc(sizeof(Utente));

        initUtente(proprietario,msg.nome,msg.password,msg.lingua,msg.funzione);
        
        
        //array che contiene i file descriptor delle pipe
        int fd[2];
        if(pipe(fd) < 0){
            perror("errore nella creazione della pipe"); 
            return "-1";
        }


        if(existStanza(listStanze,msg.nomeStanza) == 1){
            response = "-1";  

        }else{


            initStanza(tmp,utente,msg.nomeStanza,msg.direzione,fd);

            inserisciStanza(listStanze,tmp);

            response = "1";

        }
        
      
        
    }else if (isShow(&msg)){
      
        response = showStanze(listStanze);

        
        
           
    }else if (strcmp(utente->funzione,"join") == 0){
        
        //To do


        //msg.nomeStanza
      
    
        response = "-1";
        
    }else{

        response = "-1";

    }
    
    return response;

}


