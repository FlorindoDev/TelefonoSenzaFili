#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


#include "../Librerie/Autenticazione/auth.h"
#include "../Librerie/connessioneDB/connectionDB.h"
#include "../Librerie/Stanze/Stanze.h"
#include "../Librerie/Utente.h"
//cane
#define PORT 8080
#define BUFFER_SIZE 2048
#define PATH_EXE_PARTITA "./PartitaEXE"
 
ListStanze* listStanze = NULL;

char* controlloRichiestaUtente(const char *, Utente *);
void gestioneNuovaConnessione(int* new_socket, char* buffer, Utente * utente);
void home(Utente * );
void printStanze();
//int cont = 0;

typedef struct GestioneConnesioneArgs{
    int * socket;
    char buffer[BUFFER_SIZE];
    Utente utente;

}GestioneConnesioneArgs;

void cleanup_handler_connection(void *args){
    GestioneConnesioneArgs * arg=(GestioneConnesioneArgs *) args;
    printf("sto pulendo sono %lu\n", pthread_self());
    free(arg->socket);
    free(arg);
}

void * Thread_GestioneNuovaConnessione(void *args){
    GestioneConnesioneArgs * arg=(GestioneConnesioneArgs *) args;

    //aggunta handler per rupulire risorse allocate per il thread
    pthread_cleanup_push(cleanup_handler_connection, args);
    
    //lavoro
    gestioneNuovaConnessione(arg->socket,arg->buffer,&(arg->utente));
    //listStanze ? printStanze() : 0;
    printStanze();

    //utilizzo del henadler prima della chiusura
    pthread_cleanup_pop(1);  
    pthread_exit(NULL);
}

GestioneConnesioneArgs * initArg(int * new_socket){
    GestioneConnesioneArgs * tmp = (GestioneConnesioneArgs *)malloc(sizeof(GestioneConnesioneArgs));
    tmp->socket=new_socket;
    return tmp;
}


//provisoria
void printStanze(){
    pthread_mutex_lock(&(listStanze->light));
    Stanza * s = listStanze->next;
    printf("[LISTA]");
    while(s != NULL){
        printf(" ->%s(%s) ",s->nomeStanza,s->proprietario.nome);
        s = s->next;
    }
    
    printf(" -|\n");
    pthread_mutex_unlock(&(listStanze->light));
    
}

void gestioneNuovaConnessione(int* new_socket, char* buffer, Utente * utente){

    // Riceve il messaggio dal client
    read(*new_socket, buffer, BUFFER_SIZE);
    printf("\nElaborazione della stringa ricevuta...\n");
    printf("\nRicevuto %s, sono %lu\n", buffer, pthread_self());
    // Gestisce la stringa separata da ":"
    const char *response;
        
    //mi faccio dare la risposta e gli do l'utente per riempirlo
    response = controlloRichiestaUtente(buffer, utente);
    
    printf("MEssaggio da inviare %s\n", response);
    send(*new_socket, response, strlen(response), 0);
    // Chiudi la connessione
    close(*new_socket);

}

int mainServer() {

    //inzializza la lista delle stanze
    listStanze=initTesta();

    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    //char buffer[BUFFER_SIZE] = {0};

    
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
    
    printf("Server in ascolto sulla porta %d...\n", PORT);

    while (1){
        //Utente utente;
        
        printf("\nServer in atessa di un client...\n");

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

char* controlloRichiestaUtente(const char *input, Utente * utente) {

    char copia[BUFFER_SIZE];
    char* response = "";
    char *saveptr;
    strncpy(copia, input, BUFFER_SIZE); // Copia la stringa ricevuta per non modificarla direttamente
    
    // Tokenizzazione della stringa con ":" come separatore
    char *token = strtok_r(copia, ":", &saveptr);
    int parte = 1;

    printf("QUI...1\n");
    
    //Estraggo la funzione che desidera fare il client : login o signup
    strcpy(utente->funzione,token);
    printf("%s\n",utente->funzione);

    //Estraggo il nome utente passato
    token = strtok_r(NULL, ":", &saveptr);
    strcpy(utente->nome,token);
    printf("%s\n",utente->nome);

    //Estraggo la password passata
    token = strtok_r(NULL, ":", &saveptr);
    strcpy(utente->password,token);
    printf("%s\n",utente->password);

    //Apro la connessione con il db
    //PGconn* conn = strcmp(utente->funzione,"create") ? NULL : connect_to_DB();
    PGconn* conn = NULL;

    //Operazione di registrazione
    if(strcmp(utente->funzione,"signup") == 0){
        
        conn = connect_to_DB();
        //Se sto effettuando la registrazione estraggo anche la lingua
        token = strtok_r(NULL, ":", &saveptr);
        strcpy(utente->lingua,token);
        
        //Chiedo al db di registrare e controllo
        response = register_user(conn,utente) ? "1" : "-1";
        PQfinish(conn);

    //Operazione di login
    }else if (strcmp(utente->funzione,"login") == 0){
        
        conn = connect_to_DB();
        //Chiedo al db di fare il login e controllo
        response = login(conn,utente) ? "1" : "-1";
        PQfinish(conn);

    }else if (strcmp(utente->funzione,"create") == 0){
        
        Stanza * tmp = (Stanza *) malloc(sizeof(Stanza));
        Utente * proprietario = (Utente *) malloc(sizeof(Utente));
        
        token = strtok_r(NULL, ":", &saveptr);
        //printf("sono %lu, la mia stringa è `%s` - il token è `%s`\n", pthread_self(), input,token);
        //printf("%p-%p\n",utente->lingua,token);
        strcpy(utente->lingua,token);
        

        token = strtok_r(NULL, ":", &saveptr);
        strcpy(tmp->nomeStanza,token);

        strcpy(tmp->proprietario.nome,utente->nome);
        strcpy(tmp->proprietario.password,utente->password);
        strcpy(tmp->proprietario.lingua,utente->lingua);
        tmp->listaPartecipanti = &(tmp->proprietario);

        token = strtok_r(NULL, ":", &saveptr);
        tmp->direzione = atoi(token);
        
        int fd[2];
        if(pipe(fd) < 0){perror("pipe fallita come te"); return "-1";}
        write(fd[1],tmp,sizeof(Stanza));

        if(existStanza(listStanze,tmp) == 1){
            response = "-1";  

        }else{

        
            pid_t pid = fork();
            if(pid == 0){
                char fd_str[32];
                char fd_str2[32];
                sprintf(fd_str, "%d", fd[0]);
                sprintf(fd_str2, "%d", fd[1]);
                printf("Avvio partita...\n");
                execlp(PATH_EXE_PARTITA, "PartitaEXE", fd_str, fd_str2 ,NULL);
            }
            sleep(1);
            struct sockaddr_in server_addr_stanza;
            memset(&server_addr_stanza, 0, sizeof(server_addr_stanza));

            read(fd[0],&(server_addr_stanza.sin_port),sizeof(server_addr_stanza.sin_port));
            printf("porta: %d\n", ntohs(server_addr_stanza.sin_port));
            tmp->port=ntohs(server_addr_stanza.sin_port);
            tmp->pid_proccesso_stanza=pid;
            inserisciStanza(listStanze,tmp);

            response = "1";

        }
        
      
        
    }else if (strcmp(utente->funzione,"show") == 0){
      
        response = showStanze(listStanze);

        
        
           
    }else if (strcmp(utente->funzione,"join") == 0){
        
        //To do
      
        

        
        response = "-1";
        
    }else{

        response = "-1";

    }
    
    return response;

}



void home(Utente * utente){
    printf("Sono nella Home\n");


    printf("Esco dalla home\n");

}

