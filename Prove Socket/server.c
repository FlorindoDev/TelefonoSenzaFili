#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024


void home(Utente utente){


}

char gestisciStringa(const char *input) {
    char ret;
    char copia[BUFFER_SIZE];
    strncpy(copia, input, BUFFER_SIZE); // Copia la stringa ricevuta per non modificarla direttamente
    
    PGconn* conn=connect_to_DB();

    // Tokenizzazione della stringa con ":" come separatore
    char *token = strtok(copia, ":");
    int parte = 1;
    Utente utente;
    printf("QUI...1\n");
    strcpy(utente.funzione,token);
    printf("%s\n",utente.funzione);

    token = strtok(NULL, ":");
    strcpy(utente.nome,token);
    printf("%s\n",utente.nome);

    token = strtok(NULL, ":");
    strcpy(utente.password,token);
    printf("%s\n",utente.password);

    if(strcmp(utente.funzione,"signup") == 0){
        token = strtok(NULL, ":");
        strcpy(utente.lingua,token);
        
        ret =  register_user(conn,&utente) ? 's' : 'n';
        home(utente);

    }else if (strcmp(utente.funzione,"login") == 0)
    {
        printf("login qui\n");
        ret = login(conn,&utente) ? 'l' : 'n';
        home(utente);
    }

    PQfinish(conn);
    return ret;



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
        char c = gestisciStringa(buffer);
        printf("%c\n",c);
        response = "";
        if(c == 's'){

            // Risposta al client
            response = "Registrato";

        }else if (c == 'l'){
            response = "Login";
        }else{

            response = "Qualcosa è andato storto";
        }

        printf("MEssaggio da inviare %s\n", response);
        send(new_socket, response, strlen(response), 0);

        // Chiudi la connessione
        close(new_socket);


    }
    
    
    close(server_fd);

    return 0;
}