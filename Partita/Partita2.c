#include "Partita2.h"


//varabbili per la connessione
int server_fd;
struct sockaddr_in server_addr;
socklen_t addr_len = sizeof(server_addr);

Stanza stanza_corrente;

int AperturaSocket();

int main(int argc, char *argv[]){
    
    int pipe_read = atoi(argv[1]);
    int pipe_write = atoi(argv[2]);
    read(pipe_read, &stanza_corrente, sizeof(Stanza));
    AperturaSocket();
    printf("porta stanza: %d\n", ntohs(server_addr.sin_port));

    write(pipe_write,&(server_addr.sin_port),sizeof(server_addr.sin_port));


    //thread partita
    /*
    pthread_t thread;
    if (pthread_create(&thread, NULL, Thread_Game,NULL) != 0) {
        perror("Errore nella creazione del thread");
        return -1;
    }
    pthread_detach(thread);
    */
    
    // Metti il server in ascolto
    
    if (listen(server_fd, 5) < 0) {
        perror("Errore nella listen");
        close(server_fd);
        return -1;
    }
    
    
    printf("La partita è in ascolto sulla porta %d...\n", ntohs(server_addr.sin_port));

    while (1){
    
        assignConnectionToThread(server_fd,server_addr,addr_len);
        
    }

    

    close(server_fd);

    return 0;

}

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

void * Thread_GestioneNuovaConnessione(void *args){

    GestioneConnessioneArgs * arg=(GestioneConnessioneArgs *) args;
    
   
    pthread_cleanup_push(cleanup_handler_connection, args);

    //Legge info del utente che si sta connetento
    read(*(arg->socket), arg->buffer, sizeof(arg->buffer));

    //buffer è il buffer dell utente appena connesso
    Message msg = dividiStringa(arg->buffer,":",sizeof(arg->buffer));

    initUtente(&(arg->utente),msg.nome,msg.password,msg.lingua,msg.funzione,*(arg->socket));
    
    //assegna al utente il thread che lo gestirà 
    setPthread(&(arg->utente),pthread_self());

    //lavoro
    ChooseAction(arg->socket,arg->buffer,&(arg->utente));

    

    //utilizzo del henadler prima della chiusura
    pthread_cleanup_pop(1);  
    pthread_exit(NULL);
}

void entroInAttesa(){
    pthread_mutex_lock(&mutex_stato);
    pthread_cond_wait(&cond_stato,&mutex_stato);
    pthread_mutex_unlock(&mutex_stato);
}

void addPlayer(Utente * utente){
    setNextInOrder(&stanza_corrente,utente);

}

void addNameToMessage(char * message, Utente * utente){
    strcat(message,"<");
    strcat(message,utente->nome);
    strcat(message,">: ");
}

void enterInChat(int * socket, char * buffer, Utente * utente){

    char join_message[BUFFER_SIZE]="";
    strcat(join_message,utente->nome);
    strcat(join_message," joined in the chat");
    broadcast(socket,join_message);

    char message[BUFFER_SIZE] = "";
    while(getStato(&stanza_corrente,&mutex_stato) != INIZIATA){
        strcpy(message,"");
        addNameToMessage(message, utente);
        strcat(message,riceviRisposta(*socket,buffer,strlen(buffer)));
        broadcast(socket,message);
        
    }

    
}

void broadcast(int * sender_socket, char * sender_messagge){

    pthread_mutex_lock(&(stanza_corrente.light));
    Utente * in_esame = stanza_corrente.listaPartecipanti;

    for(int i=0;i<stanza_corrente.num_players;i++){

        int user_socket = getUserSocket(in_esame);
        if(user_socket == -1)continue;

        if(sender_socket != NULL){
            if(user_socket != *sender_socket) send(user_socket, sender_messagge, strlen(sender_messagge), 0);
        }else{
            send(user_socket, sender_messagge, strlen(sender_messagge), 0); 
        }
        in_esame = getNext(in_esame);
        
    }

    pthread_mutex_unlock(&(stanza_corrente.light));
}

void ChooseAction(int * socket, char * buffer, Utente * utente){

    if(getStato(&stanza_corrente, &mutex_stato) == INIZIATA){
        entroInAttesa();
    }
    addPlayer(utente);
    enterInChat(socket, buffer,utente);
}