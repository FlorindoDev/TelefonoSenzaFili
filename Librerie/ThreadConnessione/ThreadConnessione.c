
GestioneConnesioneArgs * initArg(int * new_socket){
    GestioneConnesioneArgs * tmp = (GestioneConnesioneArgs *)malloc(sizeof(GestioneConnesioneArgs));
    tmp->socket=new_socket;
    return tmp;
}


void cleanup_handler_connection(void *args){
    GestioneConnesioneArgs * arg=(GestioneConnesioneArgs *) args;
    printf("sto pulendo sono %lu\n", pthread_self());
    free(arg->socket);
    free(arg);
}


void assignConnectionToThread(int server_fd, struct sockaddr_in address, int addrlen){
    printf("\nServer in atessa di un client...\n");
    int *new_socket = malloc(sizeof(int));


    // Accetta la connessione da un client
    if ((*new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Errore nell'accept");
        
    }else{
        printf("client accettato...\n");
        pthread_t thread;
        
        GestioneConnesioneArgs * arg = initArg(new_socket);
        if (pthread_create(&thread, NULL, Thread_GestioneNuovaConnessione, arg) != 0) {
            perror("Errore nella creazione del thread");
            return;
        }
    
        pthread_detach(thread);
        
    }
}