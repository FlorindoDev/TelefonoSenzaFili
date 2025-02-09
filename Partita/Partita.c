#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#include "../Librerie/Stanze/Stanze.h"
#include "../Librerie/ThreadConnessione/ThreadConnessione.h"
#include "../Librerie/MessageEditor/MessageEditor.h"
#include "../Librerie/GestioneConnessione/GestioneConnessione.h"

#define MAX_PAROLA 5000

pthread_cond_t cond_stato = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_stato = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_chat = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_chat = PTHREAD_MUTEX_INITIALIZER;

int server_fd;
struct sockaddr_in server_addr;
socklen_t addr_len = sizeof(server_addr);
Stanza stanza_corrente;
int flag = 0;

int AperturaSocket();
void timerHomeMade(int , int); //tempo , quante volte 
void * Thread_Game(void *args);
void Game();
void ThreadKill();
void CloseThread(int input);


void gestioneNuovaConnessione(int * socket, char * buffer, Utente * utente, Message msg);

int main(int argc, char *argv[]){

    int pipe_read = atoi(argv[1]);
    int pipe_write = atoi(argv[2]);
    read(pipe_read, &stanza_corrente, sizeof(Stanza));
    AperturaSocket();
    printf("porta stanza: %d\n", ntohs(server_addr.sin_port));

    write(pipe_write,&(server_addr.sin_port),sizeof(server_addr.sin_port));


    pthread_t thread;
    if (pthread_create(&thread, NULL, Thread_Game,NULL) != 0) {
        perror("Errore nella creazione del thread");
        return -1;
    }
    pthread_detach(thread);
    

    
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

/* void CloseThread(int input){
    if(removeUtenteFromThread(stanza_corrente.listaPartecipanti,pthread_self(),stanza_corrente.num_players) == 0){
        exit(0);
    }else{
        stanza_corrente.num_players--;
    }
} */

void UnlockChat(int input){
    printf("UnlockChat\n");
    //chatParty(NULL,NULL,NULL);
    flag=1;
}

//fare che quando uno chiuda la connessione il thread toglie l'utente
void * Thread_GestioneNuovaConnessione(void *args){
    GestioneConnessioneArgs * arg=(GestioneConnessioneArgs *) args;
    
    struct sigaction sa;
    sa.sa_handler = UnlockChat;

    sa.sa_flags = 0; // Assicura che read venga interrotta
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    /* struct sigaction sa2;
    sa2.sa_handler = CloseThread;

    sa2.sa_flags = 0; // Assicura che read venga interrotta
    sigemptyset(&sa2.sa_mask);
    sigaction(SIGUSR2, &sa2, NULL); */


    pthread_cleanup_push(cleanup_handler_connection, args);

    printf("%d\n",*(arg->socket));
    read(*(arg->socket), arg->buffer, sizeof(arg->buffer));

    Message msg = dividiStringa(arg->buffer,":",sizeof(arg->buffer));

    initUtente(&(arg->utente),msg.nome,msg.password,msg.lingua,msg.funzione,*(arg->socket));
    setPthread(&(arg->utente),pthread_self());

    //lavoro
    gestioneNuovaConnessione(arg->socket,arg->buffer,&(arg->utente),msg);


    //utilizzo del henadler prima della chiusura
    pthread_cleanup_pop(1);  
    pthread_exit(NULL);
}

 
void addPlayerToParty(Utente * utente){

    //pthread_mutex_lock(&mutex_stato);
    printf("prima culo\n");
    if(getStato(&stanza_corrente, &mutex_stato) == INIZIATA){
        pthread_mutex_lock(&mutex_stato);
        pthread_cond_wait(&cond_stato,&mutex_stato);
        pthread_mutex_unlock(&mutex_stato);
        printf("ho finito di aspettare\n");
    }
    printf("dopo culo\n");
    setNextInOrder(&stanza_corrente, utente);
    printf("dopo set\n");
    
    //pthread_mutex_unlock(&mutex_stato);

}

void broadcast(int * sender_socket, char * sender_messagge){

    pthread_mutex_lock(&(stanza_corrente.light));
    Utente * in_esame = stanza_corrente.listaPartecipanti;
    printf("prima del for\n");
    for(int i=0;i<stanza_corrente.num_players;i++){
        printf("nel for read\n");
        int user_socket = getUserSocket(in_esame);
        if(sender_socket != NULL){
            if(user_socket != *sender_socket) send(user_socket, sender_messagge, strlen(sender_messagge), 0);
        }else{
            send(user_socket, sender_messagge, strlen(sender_messagge), 0); 
        }
        in_esame = getNext(in_esame);
        
    }
    printf("fine for read\n");
    pthread_mutex_unlock(&(stanza_corrente.light));
}



void chatParty(int * socket, char * buffer, Utente * utente){
    
    if(socket==NULL)return;
    //aggiusta
    char buffer2[BUFFER_SIZE]="";
    strcat(buffer2,utente->nome);
    strcat(buffer2," joined in the chat");
    broadcast(socket,buffer2);

    while(getStato(&stanza_corrente, &mutex_stato) == SOSPESA){


        printf("prima read\n");
        printf("%d\n",*(socket));
        riceviRisposta(*socket, buffer, BUFFER_SIZE);
        if(flag){
            flag = 0;
            return;
        }
        printf("dopo read\n");

        //aggiusta
        strcpy(buffer2,"");
        strcat(buffer2,"<");
        strcat(buffer2,utente->nome);
        strcat(buffer2,">: ");
        strcat(buffer2,buffer);

        broadcast(socket,buffer2);
        printf("dopo brodcats\n");
    }
    
}



void timerHomeMade(int time, int intervallo){
    char message[BUFFER_SIZE]="Rimmangono ";
    char tempo_rimanente[BUFFER_SIZE];
    for(int i=0;i<time;i+=intervallo){
        sprintf(tempo_rimanente, "%d", (time-i));
        strcat(message,tempo_rimanente);
        strcat(message," secondi");
        broadcast(NULL,message);
        strcpy(message, "Rimmangono ");
        sleep(intervallo);
    }
}

void * Thread_Game(void *args){

    char bufferPartita[BUFFER_SIZE];
    while(getStato(&stanza_corrente, &mutex_stato) != FINITA){
        printf("prima del gioco\n");
        Game();
        printf("dopo del gioco\n");
    }

}

void ThreadKill(){

    pthread_mutex_lock(&(stanza_corrente.light));
    Utente * in_esame = stanza_corrente.listaPartecipanti;
    for(int i=0;i<stanza_corrente.num_players;i++){
        pthread_kill(in_esame->thread,SIGUSR1);
        in_esame = getNext(in_esame);
        if(in_esame == NULL){
            pthread_mutex_unlock(&(stanza_corrente.light));
            return;
        }

    }
    pthread_mutex_unlock(&(stanza_corrente.light));
}

void Game(){

    while(stanza_corrente.num_players < 2);
    timerHomeMade(20,5);
    ThreadKill();
    sleep(1);

    setIniziata(&stanza_corrente, &mutex_stato);

    pthread_mutex_lock(&(stanza_corrente.light));

    Utente * in_esame = stanza_corrente.listaPartecipanti;
    char parola[MAX_PAROLA]="";
    char tmp_parola[MAX_PAROLA]="";
    int user_socket = getUserSocket(in_esame);
    for(int i=0;i<stanza_corrente.num_players;i++){
        send(user_socket, "\nè il tuo turno:\n", strlen("\nè il tuo turno:\n"), 0);
        strcpy(tmp_parola,parola);
        riceviRisposta(user_socket,parola,MAX_PAROLA);
        strcat(tmp_parola,parola);
        in_esame = getNext(in_esame);
        if(in_esame != NULL){
            //tradure parola arrivata 
            user_socket = getUserSocket(in_esame);
            
            send(user_socket, tmp_parola, strlen(tmp_parola), 0);
            printf("parola finale %s: \n", tmp_parola);
        }
    }
    pthread_mutex_unlock(&(stanza_corrente.light));
    setSospesa(&stanza_corrente, &mutex_stato);

    broadcast(NULL,tmp_parola);
    pthread_cond_broadcast(&cond_stato);
    pthread_cond_broadcast(&cond_chat);
    //sleep(5); //forse è vitale
    
}

void gestioneNuovaConnessione(int * socket, char * buffer, Utente* utente,Message msg){

    printf("prima del add\n");
    printUtente(utente);
    addPlayerToParty(utente);
    printf("dopo del add\n");
    while(getStato(&stanza_corrente, &mutex_stato) != FINITA){
        printf("sto nel  while\n");
        if(getStato(&stanza_corrente, &mutex_stato) == SOSPESA){
            printf("sto nel if del while\n");
            chatParty(socket,buffer,utente);
            printf("fine chat\n");
        }
        pthread_mutex_lock(&mutex_chat);
        pthread_cond_wait(&cond_chat,&mutex_chat);
        pthread_mutex_unlock(&mutex_chat);
        printf("mi sono riattivato\n");
    }
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


