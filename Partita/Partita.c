#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../Librerie/Stanze/Stanze.h"



int server_fd;
struct sockaddr_in server_addr;
socklen_t addr_len = sizeof(server_addr);

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
    Stanza s;
    int pipe_read = atoi(argv[1]);
    int pipe_write = atoi(argv[2]);
    read(pipe_read, &s, sizeof(Stanza));
    printf("porco dio: %s\n", s.nomeStanza);
    
    AperturaSocket();
    write(pipe_write,&(server_addr.sin_port),sizeof(server_addr.sin_port));
    


    // Metti il server in ascolto
    if (listen(server_fd, 5) < 0) {
        perror("Errore nella listen");
        close(server_fd);
        return -1;
    }
    
    printf("Server in ascolto sulla porta %d...\n", ntohs(server_addr_stanza.sin_port));

    while (1){
        //Utente utente;
        
        printf("\nSta in atessa di un client...\n");

        int *new_socket = malloc(sizeof(int));
        // Accetta la connessione da un client
        if ((*new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Errore nell'accept");
            
        }else{
            printf("client accettato %d...\n", PORT);
            pthread_t thread;
            
            GestioneConnesioneArgs * arg = initArg(new_socket);
            if (pthread_create(&thread, NULL, Thread_GestioneNuovaConnessione, arg) != 0) {
                perror("Errore nella creazione del thread");
                return 1;
            }
        
            pthread_detach(thread);
            
        }
        

    }




    close(server_fd);
    return 0;
}


