

int creaSocketByHostName(struct sockaddr_in * server_addr, int port,char * HostName){
    int socket_r;
    // Creazione della socket
    if ((socket_r = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("ERRORE : Errore nella creazione della socket");
        return -1;
    }else{
        printf("SUCCESSO : Socket creata\n");
    }

    struct hostent *server;

    server = gethostbyname(HostName);
	if (server == NULL){		 
		close(socket_r);
		return 0;
	}

    //bzero((char *) server_addr, sizeof(server_addr)); //setta tutto a zero
    
    // Configurazione dell'indirizzo del server
    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port);

    bcopy((char *)server->h_addr, (char *)server_addr->sin_addr.s_addr, server->h_length); 
    
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



int creaSocket(struct sockaddr_in * server_addr, int port,char * ip){
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
    server_addr->sin_addr.s_addr = inet_addr(ip);
    
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

char* riceviRispostaSignal(int socket, char * buffer, int grandezza_buffer/* , pthread_key_t flag_key */){

    /* int *flag_uscita = (int *)pthread_getspecific(flag_key); */
    int charPassati = read(socket, buffer, grandezza_buffer);
    printf("char passati %d:\n", charPassati);

    
    /* if(charPassati <= 0 && *flag_uscita==0){

        printf("pirma del flag %p:\n", flag_uscita);
        *flag_uscita=0;

        pthread_kill(pthread_self(),SIGUSR2);
    } */

    if (charPassati < grandezza_buffer){
        buffer[charPassati]='\0';
    }
    printf("Risposta dal server: %s\n", buffer);

    if(strcmp(buffer, EXIT_MESSAGE) == 0){
        buffer="";
        pthread_kill(pthread_self(),SIGUSR2);
    }

    
    //printf("Risposta dal server: %s\n", buffer);
    //printf("char passati %d:\n", charPassati);
    return buffer;

}


char* riceviRispostaGame(int socket, char * buffer, int grandezza_buffer, pthread_key_t flag_key_game, pthread_t thread_user ){

    int *flag_uscita = (int *)pthread_getspecific(flag_key_game); 
    int charPassati = read(socket, buffer, grandezza_buffer);
    printf("char passati %d:\n", charPassati);

    
    /* if(charPassati <= 0){
        printf("Sto facendo il segnale %d:\n", charPassati);
        *flag_uscita=1;
        pthread_kill(pthread_self(),SIGUSR2);
    } */

    if (charPassati < grandezza_buffer){
        buffer[charPassati]='\0';
    }
    printf("Risposta dal server: %s\n", buffer);

    if(strcmp(buffer, EXIT_MESSAGE) == 0){

        buffer="";
        *flag_uscita=1;
        printf("io sono self %ld sto uccidendo %ld\n", pthread_self(), thread_user);
        pthread_kill(thread_user,SIGUSR2);
        printf("dopo pthread_kill\n");
    }


    //printf("Risposta dal server: %s\n", buffer);
    printf("char passati %d:\n", charPassati);
    return buffer;

}


char* riceviRisposta(int socket, char * buffer, int grandezza_buffer){

    int charPassati = read(socket, buffer, grandezza_buffer);
    
    if (charPassati < grandezza_buffer){
        buffer[charPassati]='\0';
    }
    printf("Risposta dal server: %s\n", buffer);
    printf("char passati %d:\n", charPassati);
    return buffer;
}



void mandaMessaggio(int socket, char * message){

    // Invia il messaggio al server
    send(socket, message, strlen(message), 0);
    printf("Messaggio inviato: %s\n", message);
    

}
