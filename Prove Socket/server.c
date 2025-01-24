#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../include/auth.h"
#include "../include/connectionDB.h"

#define PORT 8080
#define BUFFER_SIZE 1024

char* controlloRichiestaUtente(const char *, Utente *);
void home(Utente * );

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

        // Riceve il messaggio dal client
        read(new_socket, buffer, BUFFER_SIZE);
        printf("\nElaborazione della stringa ricevuta...\n");

        // Gestisce la stringa separata da ":"
        const char *response;
        
        //mi faccio dare la risposta e gli do l'utente per riempirlo
        response = controlloRichiestaUtente(buffer, &utente);
        
        printf("MEssaggio da inviare %s\n", response);
        send(new_socket, response, strlen(response), 0);

        // Chiudi la connessione
        close(new_socket);

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
        if(register_user(conn,utente) == 1){
            response = "1";
        }else{
            response = "-1";
    
        }
    //Operazione di login
    }else if (strcmp(utente->funzione,"login") == 0){
        
        //Chiedo al db di fare il login e controllo
        if(login(conn,utente) == 1){
            response = "1";
        }else{
            response = "-1";
        }
        
    }else{
        response = "-1";

    }


    PQfinish(conn);
    
    return response;

}

void home(Utente * utente){
    printf("Sono nella Home\n");


    printf("Esco dalla home\n");

}
