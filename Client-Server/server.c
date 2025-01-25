#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>



#include "../Librerie/Autenticazione/auth.h"
#include "../Librerie/connessioneDB/connectionDB.h"
#include "../Librerie/Stanze/Stanze.h"
#include "../Librerie/Utente.h"

#define PORT 8080
#define BUFFER_SIZE 1024

char* controlloRichiestaUtente(const char *, Utente *);
void inserisciStanza(Stanza * );
int existStanza(Stanza * );

void home(Utente * );
int existStanza(Stanza * );


Stanza * stanze = NULL;

void printStanze(){
    Stanza * s = stanze;
    printf("[LISTA]");
    while(s != NULL){
        printf(" ->%s(%s) ",s->nomeStanza,s->proprietario.nome);
        s = s->next;
    }
    
    printf(" -|\n");
    
}

void gestioneNuovaConnessione(int* new_socket, char* buffer, Utente * utente){

    // Riceve il messaggio dal client
    read(*new_socket, buffer, BUFFER_SIZE);
    printf("\nElaborazione della stringa ricevuta...\n");
    // Gestisce la stringa separata da ":"
    const char *response;
        
    //mi faccio dare la risposta e gli do l'utente per riempirlo
    response = controlloRichiestaUtente(buffer, utente);
    
    printf("MEssaggio da inviare %s\n", response);
    send(*new_socket, response, strlen(response), 0);
    // Chiudi la connessione
    close(*new_socket);

}

int mainServer() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    
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

    while (1){
        Utente utente;
        
        printf("Server in ascolto sulla porta %d...\n", PORT);

       
        // Accetta la connessione da un client
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Errore nell'accept");
            close(server_fd);
            return -1;
        }

        gestioneNuovaConnessione(&new_socket,buffer,&utente);
      
        
        
        printStanze();

    }
    
    
    close(server_fd);

    return 0;
}

char* controlloRichiestaUtente(const char *input, Utente * utente) {

    char copia[BUFFER_SIZE];
    char* response = "";
    strncpy(copia, input, BUFFER_SIZE); // Copia la stringa ricevuta per non modificarla direttamente
    
    
    // Tokenizzazione della stringa con ":" come separatore
    char *token = strtok(copia, ":");
    int parte = 1;

    printf("QUI...1\n");
    
    //Estraggo la funzione che desidera fare il client : login o signup
    strcpy(utente->funzione,token);
    printf("%s\n",utente->funzione);

    //Estraggo il nome utente passato
    token = strtok(NULL, ":");
    strcpy(utente->nome,token);
    printf("%s\n",utente->nome);

    //Estraggo la password passata
    token = strtok(NULL, ":");
    strcpy(utente->password,token);
    printf("%s\n",utente->password);

    //Apro la connessione con il db
    PGconn* conn = connect_to_DB();

    //Operazione di registrazione
    if(strcmp(utente->funzione,"signup") == 0){
        
        //Se sto effettuando la registrazione estraggo anche la lingua
        token = strtok(NULL, ":");
        strcpy(utente->lingua,token);
        
        //Chiedo al db di registrare e controllo
        response = register_user(conn,utente) ? "1" : "-1";

        
    //Operazione di login
    }else if (strcmp(utente->funzione,"login") == 0){
        
        //Chiedo al db di fare il login e controllo
        response = login(conn,utente) ? "1" : "-1";
        
    }else if (strcmp(utente->funzione,"create") == 0){
        
        Stanza * tmp = (Stanza *) malloc(sizeof(Stanza));
        Utente * proprietario = (Utente *) malloc(sizeof(Utente));
        
        token = strtok(NULL, ":");
        strcpy(utente->lingua,token);
      
        token = strtok(NULL, ":");
        strcpy(tmp->nomeStanza,token);

        strcpy(tmp->proprietario.nome,utente->nome);
        strcpy(tmp->proprietario.password,utente->password);
        strcpy(tmp->proprietario.lingua,utente->lingua);
        tmp->listaPartecipanti = &(tmp->proprietario);

        if(existStanza(tmp) == 0){
            response = "-1";   
        }else{
            inserisciStanza(tmp);

            response = "1";
        }

      
        
    }else{

        response = "-1";

    }


    PQfinish(conn);
    
    return response;

}

void inserisciStanza(Stanza * tmp){
    if(stanze != NULL){
        tmp->next = stanze;
        stanze = tmp;
    }else{
        stanze = tmp;
    }

}



void home(Utente * utente){
    printf("Sono nella Home\n");


    printf("Esco dalla home\n");

}

int existStanza(Stanza * s){
    if(stanze != NULL){
        Stanza * tmp = stanze;
        while(tmp != NULL){
            if(strcmp(tmp->nomeStanza,s->nomeStanza) == 0)
                return 0;
            tmp = tmp->next;
        }
    }

    return -1;
}
