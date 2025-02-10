
GestioneConnessioneArgs * initArg(int * new_socket){
    GestioneConnessioneArgs * tmp = (GestioneConnessioneArgs *)calloc(sizeof(GestioneConnessioneArgs),1);
    tmp->socket=new_socket;
    return tmp;
}

GestioneConnessioneArgs * initArgUtente(int * new_socket, Utente utente){
    GestioneConnessioneArgs * tmp = (GestioneConnessioneArgs *)calloc(sizeof(GestioneConnessioneArgs),1);
    tmp->socket=new_socket;
    tmp->utente=utente;
    return tmp;
}


void cleanup_handler_connection(void *args){
    GestioneConnessioneArgs * arg=(GestioneConnessioneArgs *) args;
    printf("sto pulendo sono %lu\n", pthread_self());
    free(arg->socket);
    free(arg);
}


void assignConnectionToThread(int server_fd, struct sockaddr_in address, socklen_t addrlen){
    printf("\nServer in atessa di un client...\n");
    int *new_socket = malloc(sizeof(int));


    // Accetta la connessione da un client
    if ((*new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Errore nell'accept");
        
    }else{
        printf("client accettato...\n");
        pthread_t thread;
        
        GestioneConnessioneArgs * arg = initArg(new_socket);
        if (pthread_create(&thread, NULL, Thread_GestioneNuovaConnessione, arg) != 0) {
            perror("Errore nella creazione del thread");
            return;
        }
    
        pthread_detach(thread);
        
    }
}