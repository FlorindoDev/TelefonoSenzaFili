#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

typedef struct Utente{
    char* nome;
    char* password;
    char* lingua;
    char* funzione;
    struct Utente * next;
}Utente;


void home(Utente utente){


}

char gestisciStringa(const char *input) {
    char copia[BUFFER_SIZE];
    strncpy(copia, input, BUFFER_SIZE); // Copia la stringa ricevuta per non modificarla direttamente

    // Tokenizzazione della stringa con ":" come separatore
    char *token = strtok(copia, ":");
    int parte = 1;
    Utente utente;

    strcpy(utente.funzione,token);
    token = strtok(NULL, ":");
    strcpy(utente.nome,token);
    token = strtok(NULL, ":");
    strcpy(utente.password,token);
    
    if(strcmp(utente.funzione,"signup")){
        token = strtok(NULL, ":");
        strcpy(utente.lingua,token);

        home(utente);
        return 's';

    }
    
    
    return 'l';



}

int main() {
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
        if(gestisciStringa(buffer) == 's'){

            // Risposta al client
            response = "Registrazione fallita\n";

        }else{

            response = "Login fallita\n";
        }

        
        send(new_socket, response, strlen(response), 0);

        // Chiudi la connessione
        close(new_socket);


    }
    
    
    close(server_fd);

    return 0;
}