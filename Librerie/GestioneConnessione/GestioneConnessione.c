

int creaSocket(struct sockaddr_in * server_addr, int port){
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
    server_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
    
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

char* riceviRispostaSignal(int socket, char * buffer, int grandezza_buffer){

    int charPassati = read(socket, buffer, grandezza_buffer);

    if (charPassati < grandezza_buffer){
        buffer[charPassati]='\0';
    }


    if(strcmp(buffer, EXIT_MESSAGE) == 0){
        buffer="";
        pthread_kill(pthread_self(),SIGUSR2);
    }

    return buffer;

}


char* riceviRispostaGame(int socket, char * buffer, int grandezza_buffer, pthread_key_t flag_key_game, pthread_t thread_user ){

    int *flag_uscita = (int *)pthread_getspecific(flag_key_game); 
    int charPassati = read(socket, buffer, grandezza_buffer);


    if (charPassati < grandezza_buffer){
        buffer[charPassati]='\0';
    }


    if(strcmp(buffer, EXIT_MESSAGE) == 0){
        buffer="";
        *flag_uscita=1;
        pthread_kill(thread_user,SIGUSR2);
      
    }

    return buffer;

}


char* riceviRisposta(int socket, char * buffer, int grandezza_buffer){

    int charPassati = read(socket, buffer, grandezza_buffer);
    
    if (charPassati < grandezza_buffer){
        buffer[charPassati]='\0';
    }

    return buffer;
}



void mandaMessaggio(int socket, char * message){

    // Invia il messaggio al server
    send(socket, message, strlen(message), 0);

    

}

void sendLingua(int *new_socket, char * ling, char * response){
    if(strcmp(response,"1")==0)send(*new_socket, ling, strlen(ling), 0);
        
    else send(*new_socket, "-1", strlen("-1"), 0);
}
