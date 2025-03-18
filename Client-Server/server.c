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
#include "../Librerie/GestioneConnessione/GestioneConnessione.h"


//il BUFFER_SIZE STA IN ThreadConnessione
#define PORT 8080
 

ListStanze* listStanze = NULL;


char* controlloRichiestaUtente(const char *, Utente *, int *);
char* registerUser(PGconn * , Utente *);
char* loginUser(PGconn * , Utente *, char*);
void gestioneNuovaConnessione(int* new_socket, char* buffer, Utente * utente);


int mainServer() {

    //inzializza la lista delle stanze
    listStanze=initTesta();


    int server_fd; //file decriptor socker server
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    
    // Creazione della socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Errore nella creazione della socket");
        return -1;
    }

    // Configurazione dell'indirizzo del server
    address.sin_family = AF_INET;

    //INADDR_ANY = 0.0.0.0 per ascoltare su qualsiasi interfaccia di rete
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

    // Gestisce la stringa separata da ":"
    const char *response;
        
    //mi faccio dare la risposta e gli do l'utente per riempirlo
    response = controlloRichiestaUtente(buffer, utente, new_socket);

    send(*new_socket, response, strlen(response), 0);
    // Chiudi la connessione
    close(*new_socket);

}

char * loginUser(PGconn* conn, Utente * utente, char * ling){
    conn = connect_to_DB();
    char * response = login(conn,utente,ling) ? "1" : "-1";
    PQfinish(conn);
    return response;
}

char * registerUser(PGconn* conn, Utente * utente){
 
    conn = connect_to_DB();
    char * response = register_user(conn,utente) ? "1" : "-1";
    PQfinish(conn);
    return response;
    
}

char* controlloRichiestaUtente(const char *input, Utente * utente, int * new_socket) {

    char ling[10]="";
    char * response = "";

    Message msg = dividiStringa(input, ":", BUFFER_SIZE);

    PGconn* conn = NULL;

    initUtente(utente,msg.nome,msg.password,msg.lingua,msg.funzione,0);

    if(isSingUp(&msg)){
        
        response = registerUser(conn, utente);


    }else if (isLogin(&msg)){
        
        response = loginUser(conn, utente, ling);
        sendLingua(new_socket, ling, response);
        

    }else if (isCreate(&msg)){
        
        Stanza * tmp = (Stanza *) calloc(sizeof(Stanza),1);
        Utente * proprietario = (Utente *) calloc(sizeof(Utente),1);

        initUtente(proprietario,msg.nome,msg.password,msg.lingua,msg.funzione,0);
        
        
        //array che contiene i file descriptor delle pipe
        int fd[2];
        if(pipe(fd) < 0){
            perror("errore nella creazione della pipe"); 
            return "-1";
        }


        if(existStanza(listStanze,msg.nomeStanza) == 1){
            response = "-1";  

        }else{

            //crea il processo figlio e exec e prende la porta del figlio
            initStanza(tmp,utente,msg.nomeStanza,msg.direzione,fd);

            inserisciStanza(listStanze,tmp);

            // per far connettere il creatore, la porta deve essere inviata subito
            char send_port[10];
            sprintf(send_port, "%hu", tmp->port);

            send(*new_socket, send_port, sizeof(send_port), 0);


            response = "1";

        }
        
      
        
    }else if (isShow(&msg)){
      
        response = showStanze(listStanze);

    }else{

        response = "-1";

    }
    
    return response;

}


