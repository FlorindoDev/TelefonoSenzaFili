

int creaSocket(struct sockaddr_in * server_addr, int port, char * indirizzo){
    int socket_r;
    // Creazione della socket
    if ((socket_r = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("ERRORE : Errore nella creazione della socket");
        return -1;
    }else{
        printf("SUCCESSO : Socket creata\n");
    }
    
    // Configurazione dell'indirizzo del server
    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port);
    server_addr->sin_addr.s_addr = inet_addr(indirizzo);
    
    // Connessione al server
    if ((connect(socket_r, (struct sockaddr*)server_addr,sizeof(*server_addr))) < 0) {
        perror("ERRORE : Errore nella connessione");
        close(socket_r);
        return -1;
    }else{
        printf("SUCCESSO : Connessione creata\n");
    }

    return socket_r;
}

int chiudiSocket(int socket){
    close(socket);
}

char* riceviRisposta(int socket, char * buffer, int grandezza_buffer){

    int charPassati = read(socket, buffer, grandezza_buffer);
    
    if (charPassati < grandezza_buffer){
        buffer[charPassati]='\0';
    }
    printf("%s\n", buffer);
    return buffer;
}



void mandaMessaggio(int socket, char * message){

    // Invia il messaggio al server
    send(socket, message, strlen(message), 0);
    

}

//serve per inviare la lingua al client dopo aver fatto il login
void sendLingua(int *new_socket, char * ling, char * response){

    if(strcmp(response,"1")==0)send(*new_socket, ling, strlen(ling), 0);
        
    else send(*new_socket, "-1", strlen("-1"), 0);

}

int isSocketConnected(int sock){

    char buff[1];
    //MSG_peek legge senza distrugere MSG_DONTWAIT non bloccante
    int result = recv(sock, buff, 1, MSG_PEEK | MSG_DONTWAIT);
    
    if (result == 0) {
        // La connessione è stata chiusa correttamente
        return 0;
    } else if (result < 0) {
        // Controlla se è un errore temporaneo
        //EAGAIN: Significa che la risorsa richiesta non è attualmente disponibile
        //EWOULDBLOCK indica che l'operazione avrebbe bloccato il processo se fosse stata eseguita in modalità bloccante, 
        //quindi in modalità non bloccante viene restituito questo errore.

        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Non ci sono dati ma la connessione è viva
            return 1;
        } else {
            // Errore di connessione
            return 0;
        }
    }
    // Ci sono dati da leggere, la connessione è attiva
    return 1;
}


int riceviRispostaConTimeout(int socket, char *buffer, size_t size, int timeout_seconds) {
   
    time_t start_time = time(NULL);
    struct pollfd fd; //pollfd è una struttura dove si inserisce il fd per il pollin
    fd.fd = socket;    
    fd.events = POLLIN; //evento da fare
    
    // Buffer per i controlli periodici
    char check_buffer[1];
    
    // Loop fino al timeout totale
    while (time(NULL) - start_time < timeout_seconds) {
        // Poll con un timeout breve (es. 2 secondi)
        int ret = poll(&fd, 1, 2000);
        
        if (ret < 0) {
            // Errore nella poll
            return 0;
        } 
        else if (ret == 0) {
            // Nessun dato disponibile, verifica se il socket è ancora connesso
            if (!isSocketConnected(socket)) {
                // Il client si è disconnesso
                return 0;
            }
            // Altrimenti continua ad aspettare
            continue;
        }
        
        // Ci sono dati da leggere
        // fd.revents conetiene una sequenza di bit  che indica cosa e successo nel file descriptor
        // fd.revents & POLLIN controlla se e accaduto un evento POLLIN
        if (fd.revents & POLLIN) {
            ssize_t bytes = recv(socket, buffer, size - 1, 0);
            if (bytes <= 0) {
                // Il client si è disconnesso
                return 0;
            }
            buffer[bytes] = '\0';
            return 1; // Successo
        }
    }
    
    // Timeout raggiunto
    return 0;
    
}
