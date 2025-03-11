#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <math.h>


#include "../Librerie/Stanze/Stanze.h"
#include "../Librerie/Utente/Utente.h"
#include "../Librerie/GestioneConnessione/GestioneConnessione.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define EXIT_MESSAGE "c0886be5e66f118ea41bd90727881825"


//Menu dopo essere loggato
void homeShow();

//Funzioni richieste dall'utente
int login();
int signUp();
int mostraStanzaGioco();
int creaStanzaGioco();
int connessionePartita();
int entraStanzaGioco();
void * Thread_ChatParty(void *args);
int chatParty();
void DisconnectedHandler(int);
void * Thread_ChatParty(void *args);

//Funzione creazione del messaggio ben formattato
void creaComando(char*  , char* );


//Variabiili globali per la gestione della socket per la socket
int sock = -1;
int socket_partita = -1;
struct sockaddr_in serv_addrr_g;
char buffer[BUFFER_SIZE] = {0};


Utente utente;
Stanza stanza;



int main() {

    /* struct sigaction sa2;
    sa2.sa_handler = DisconnectedHandler;

    sigemptyset(&sa2.sa_mask);
    sigaction(SIGINT, &sa2, NULL); */


    int scelta;
    int risultato;

    do {
        printf("\n\n=== TelefonoSenzaFili ===\n");
        printf("1. Login\n");
        printf("2. SignUp\n");
        printf("3. Esci\n");
        printf("Seleziona un'opzione: ");
        scanf("%d", &scelta);
        



        switch (scelta) {
            case 1:
                risultato = login();
                if(risultato != 1){
                    printf("ERRORE : Nome o password errata\n");
                }else{
                    printf("SUCCESSO : loggato");
                    homeShow();
                }

                break;
            case 2:
                risultato = signUp();
                if(risultato != 1){
                    printf("ERRORE : registrazione non riuscita\n");

                }else{
                    printf("SUCCESSO : registrazione riuscita\n");  
                }
                break;
            case 3:
                printf("SUCCESSO : Uscita dal programma.\n");
                break;
            default:
                printf("ERRORE : Opzione non valida. Riprova.\n");
                break;
        }
    } while (scelta != 3);

    return 0;
}


void DisconnectedHandler(int input){
    char message[BUFFER_SIZE] = EXIT_MESSAGE;
    //mandaMessaggio(socket_partita, message);
    exit(0);
}

void * Thread_ChatParty(void *args){

    char bufferPartita[BUFFER_SIZE];
    while(1){
        //int ris = riceviRispostaConTimeout(socket_partita,bufferPartita, BUFFER_SIZE,120);
        //printf("%s\n",bufferPartita);

        //if(!ris){
            //close(socket_partita);
            //break;
        //}
        riceviRisposta(socket_partita,bufferPartita,BUFFER_SIZE);
    }

    

}

int login(){

    // Chiedere il nome
    printf("Inserisci il tuo nome: ");
    scanf("%49s", utente.nome); // Limitiamo l'input a 49 caratteri per evitare buffer overflow

    // Chiedere la password
    printf("Inserisci la tua password: ");
    scanf("%49s", utente.password); // Limitiamo l'input a 49 caratteri


    char message[BUFFER_SIZE];
    creaComando(message,"login");

    sock = creaSocket(&serv_addrr_g,PORT);

    if(sock != -1){
        
        // Invia il messaggio al server
        mandaMessaggio(sock, message);
       
        // Riceve la risposta dal server
        riceviRisposta(sock,buffer,2);
        strcpy(utente.lingua,buffer);
        riceviRisposta(sock,buffer,BUFFER_SIZE);

        chiudiSocket(sock);
    
        return strcmp("-1",buffer) ? 1 : -1; //ok dal server
    }
    

    chiudiSocket(sock);
    return -1;

}

int signUp(){

    // Chiedere il nome
    printf("Inserisci il tuo nome: ");
    scanf("%49s", utente.nome); // Limitiamo l'input a 49 caratteri per evitare buffer overflow

    // Chiedere la password
    printf("Inserisci la tua password: ");
    scanf("%49s", utente.password); // Limitiamo l'input a 49 caratteri

    // Chiedere la lingua
    printf("Inserisci la tua lingua preferita: ");
    scanf("%19s", utente.lingua); // Limitiamo l'input a 19 caratteri

    char message[BUFFER_SIZE];
    creaComando(message,"singup");

    sock = creaSocket(&serv_addrr_g,PORT);
    
    if (sock != -1){

        // Invia il messaggio al server
        mandaMessaggio(sock, message);
    
        riceviRisposta(sock,buffer, BUFFER_SIZE);
        
        // Riceve la risposta dal server
        chiudiSocket(sock);

        return strcmp("-1",buffer) ? 1 : -1; //ok dal server
    }

    chiudiSocket(sock);
    return 0;
    

}

void homeShow(){
    int risultato;
    int scelta;

    do {
        printf("\n\n=== Menu ===\n");
        printf("1. Entra Stanza di Gioco\n");
        printf("2. Crea Stanza di Gioco\n");
        printf("3. LogOut\n");
        printf("Seleziona un'opzione: ");
        scanf("%d", &scelta);


        switch (scelta) {
            case 1:
                risultato = mostraStanzaGioco();
                if(risultato == -1){
                    printf("ERRORE : nessuna stanza presente");
                }else{
                    risultato = entraStanzaGioco();
                    if(risultato == -1){
                        printf("ERRORE : stanza non esistente o inaccessibile (controlla il codice)");
                    }else{
                        printf("SUCCESSO : entrato in stanza");
                        risultato = chatParty();
                    }

                }
                
                
                
                

                break;
            case 2:
                
                risultato = creaStanzaGioco();
                if(risultato == -1){
                    chiudiSocket(sock);
                    printf("ERRORE : creazione stanza di gioco fallita");
                }else{
                    printf("SUCCESSO : creazione stanza di gioco creata con successo");

                    risultato = atoi(riceviRisposta(sock, buffer, BUFFER_SIZE));
                    stanza.port = (unsigned short int)risultato;
    
                    if(risultato == -1){
                        chiudiSocket(sock);
                        printf("ERRORE : creazione stanza di gioco fallita\n");
                        
                    }else{

                        printf("%hu\n", stanza.port);
                        chiudiSocket(sock);
                        connessionePartita();
                        chatParty();

                    }
                    
            

                }

                break;
            
            case 3:
                printf("SUCCESSO : LogOut eseguito");
                break;
            
            default:
                printf("ERRORE : Opzione non valida. Riprova.\n");
                break;
        }
    } while (scelta != 3);


}

int mostraStanzaGioco(){
    
    char message[BUFFER_SIZE];
    creaComando(message,"show");

    sock = creaSocket(&serv_addrr_g,PORT);

    if(sock != -1){
        
        // Invia il messaggio al server
        mandaMessaggio(sock,message);
       
        // Riceve la risposta dal server
        riceviRisposta(sock,buffer, BUFFER_SIZE);
        
        chiudiSocket(sock);


    }else{
        chiudiSocket(sock);
        return -1;
    }
    
    chiudiSocket(sock);

    //Inizzializzazione token , se e' -1 esco
    char *token = strtok(buffer, ":");
    if(strcmp(token,"-1") == 0){
        return -1;
    }


    printf("\nSTANZE ESISTENTI: [nome (codice stanza)] \n");
    unsigned long int i = 1;
    while(token != NULL){
        printf(" %lu) %s\n",i++,token);
        token = strtok(NULL, ":");
    }
    printf("\nFINE STANZE;\n");
    
    return 1;
}


int entraStanzaGioco(){

    printf("Inserisci codice stanza in cui vuoi entrare : ");
    
    scanf("%hu", &(stanza.port));
    
    pthread_t thread;
    if (pthread_create(&thread, NULL, Thread_ChatParty,NULL) != 0) {
        perror("Errore nella creazione del thread");
        return -1;
    }
    pthread_detach(thread);
    

    char message[BUFFER_SIZE];
    creaComando(message,"join");

    socket_partita = creaSocket(&serv_addrr_g,stanza.port);
    printf("socket1: %d\n", socket_partita);

    if(socket_partita != -1){
        
        // Invia il messaggio al server
        mandaMessaggio(sock,message);
     

        return strcmp("-1",buffer) ? 1 : -1; //ok dal server
    }

    return -1;
}

int connessionePartita(){
    pthread_t thread;
    if (pthread_create(&thread, NULL, Thread_ChatParty,NULL) != 0) {
        perror("Errore nella creazione del thread");
        return -1;
    }
    pthread_detach(thread);
    

    char message[BUFFER_SIZE];
    creaComando(message,"join");

    socket_partita = creaSocket(&serv_addrr_g,stanza.port);
    printf("socket1: %d\n", socket_partita);

    if(socket_partita != -1){
        
        // Invia il messaggio al server
        mandaMessaggio(sock,message);
     

        return strcmp("-1",buffer) ? 1 : -1; //ok dal server
    }

    return -1;
    
}

int creaStanzaGioco(){
    
    printf("\n\n=== Creazione Stanza ===\n");
    
    // Chiedere il nome
    printf("Inserisci nome partita: ");
    
    //Pulire buffer
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    scanf("%[^\n]",stanza.nomeStanza);
    printf("\n(%s)\n",stanza.nomeStanza);

    stanza.stato = CREATA;

    int scelta = 0;
    do{
        // Chiedere il nome
        printf("Inserisci direzione : \n 1) Asc; \n 2) Desc ; \nRisposta : ");
        scanf("%d",&scelta);

        if(scelta != 1 && scelta != 2){
            printf("ERRORE :  metti 1 o 2; \n");
        }

    }while(scelta != 1 && scelta != 2);
    
    if(scelta == 1){
        stanza.direzione = ASC;
    }else{
        stanza.direzione = DESC;
    }


    char message[BUFFER_SIZE];
    creaComando(message,"create");

    sock = creaSocket(&serv_addrr_g,PORT);

    if(sock != -1){
        
        // Invia il messaggio al server
        mandaMessaggio(sock, message);
       
        // Riceve la risposta dal server
        //riceviRisposta(sock,buffer, BUFFER_SIZE );
        
        //chiudiSocket(sock);
        
    
        return 1;

    }
    

    chiudiSocket(sock);
    return -1;
}

void creaComando(char* message , char* funzione){

    //Copy è per inizializzare la stringa messagge , senza prenderebbe anche i vecchi messaggi(colpa di malloc)
    strcmp(funzione,"") ? strcpy(message,funzione) : strcpy(message," ");
    strcat(message,":");
    
    strcmp(utente.nome,"") ? strcat(message,utente.nome) : strcat(message," ");
    strcat(message,":");
    
    strcmp(utente.password,"") ? strcat(message,utente.password) : strcat(message," ");
    strcat(message,":");

    strcmp(utente.lingua,"") ? strcat(message,utente.lingua) : strcat(message," ");
    strcat(message,":");

    strcmp(stanza.nomeStanza,"") ? strcat(message,stanza.nomeStanza) : strcat(message," ");
    strcat(message,":");

    char dir[10];
    sprintf(dir,"%d",stanza.direzione);
    strcmp(dir,"") ? strcat(message,dir) : strcat(message," ");
    strcat(message,":");
    
    
}

int chatParty(){

    
    
    printf("\nSEI IN CHAT COGLIONE:\n");
    

    char message[BUFFER_SIZE];
    char bufferChatRicezione[BUFFER_SIZE];
    creaComando(message,"chat");

    while(1){

    
        char c;
        while ((c = getchar()) != '\n' && c != EOF);

        printf("socket2: %d\n", socket_partita);
        
        strcpy(message,"");
        scanf("%[^\n]",message);
        // Invia il messaggio al server
        mandaMessaggio(socket_partita, message);

    
        // Riceve la risposta dal server
        
    }
    
    return strcmp("-1",buffer) ? 1 : -1; //ok dal server
    
    

    //chiudiSocket();

    return -1;

}
