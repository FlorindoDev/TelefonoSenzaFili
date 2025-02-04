#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#include "../Librerie/Stanze/Stanze.h"
#include "../Librerie/Utente/Utente.h"

#define PORT 8080
#define BUFFER_SIZE 1024


//Menu dopo essere loggato
void homeShow();

//Funzioni richieste dall'utente
int login();
int signUp();
int mostraStanzaGioco();
int creaStanzaGioco();
int entraStanzaGioco();

//Funzione creazione del messaggio ben formattato
void creaComando(char*  , char* );

//Funzioni per gestione socket
int creaSocket(int);
int chiudiSocket();
char* riceviRisposta();
void mandaMessaggio(char*);

//Variabiili globali per la gestione della socket per la socket
int sock = 0;
struct sockaddr_in serv_addr;
char buffer[BUFFER_SIZE] = {0};


Utente utente;
Stanza stanza;


int main() {
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

int login(){

    // Chiedere il nome
    printf("Inserisci il tuo nome: ");
    scanf("%49s", utente.nome); // Limitiamo l'input a 49 caratteri per evitare buffer overflow

    // Chiedere la password
    printf("Inserisci la tua password: ");
    scanf("%49s", utente.password); // Limitiamo l'input a 49 caratteri


    char message[BUFFER_SIZE];
    creaComando(message,"login");

    int successo = creaSocket(PORT);

    if(successo != -1){
        
        // Invia il messaggio al server
        mandaMessaggio(message);
       
        // Riceve la risposta dal server
        riceviRisposta();
        
        chiudiSocket();
    
        return strcmp("-1",buffer) ? 1 : -1; //ok dal server
    }
    

    chiudiSocket();
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
    creaComando(message,"signup");

    int successo = creaSocket(PORT);
    
    if (successo != -1){

        // Invia il messaggio al server
        mandaMessaggio(message);
    
        riceviRisposta();
        
        // Riceve la risposta dal server
        chiudiSocket();

        return strcmp("-1",buffer) ? 1 : -1; //ok dal server
    }

    chiudiSocket();
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
                    }

                }
                
                
                
                

                break;
            case 2:
                
                risultato = creaStanzaGioco();
                if(risultato == -1){
                    printf("ERRORE : creazione stanza di gioco fallita");
                }else{
                    printf("SUCCESSO : creazione stanza di gioco creata con successo");
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

    int successo = creaSocket(PORT);

    if(successo != -1){
        
        // Invia il messaggio al server
        mandaMessaggio(message);
       
        // Riceve la risposta dal server
        riceviRisposta();
        
        chiudiSocket();


    }else{
        chiudiSocket();
        return -1;
    }
    
    chiudiSocket();

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
    

    char message[BUFFER_SIZE];
    creaComando(message,"join");

    int successo = creaSocket(stanza.port);

    if(successo != -1){
        
        // Invia il messaggio al server
        mandaMessaggio(message);
       
        // Riceve la risposta dal server
        riceviRisposta();
        

        return strcmp("-1",buffer) ? 1 : -1; //ok dal server
    }
    

    chiudiSocket();

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

    int successo = creaSocket(PORT);

    if(successo != -1){
        
        // Invia il messaggio al server
        mandaMessaggio(message);
       
        // Riceve la risposta dal server
        riceviRisposta();
        
        chiudiSocket();
    
        return strcmp("-1",buffer) ? 1 : -1;

    }
    

    chiudiSocket();
    return -1;
}




int creaSocket(int port){
    
    // Creazione della socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("ERRORE : Errore nella creazione della socket");
        return -1;
    }else{
        printf("SUCCESSO : Socket creata\n");
    }
    
    // Configurazione dell'indirizzo del server
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Connessione al server
    if ((connect(sock, (struct sockaddr*)&serv_addr,sizeof(serv_addr))) < 0) {
        perror("ERRORE : Errore nella connessione");
        close(sock);
        return -1;
    }else{
        printf("SUCCESSO : Connessione creata\n");
    }

    return 0;
}

int chiudiSocket(){
    close(sock);
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

char* riceviRisposta(){

    int charPassati = read(sock, buffer, BUFFER_SIZE);
        if (charPassati < BUFFER_SIZE){
            buffer[charPassati]='\0';
        }
    printf("Risposta dal server: %s\n", buffer);
    return buffer;
}

void mandaMessaggio(char * message){

    // Invia il messaggio al server
    send(sock, message, strlen(message), 0);
    printf("Messaggio inviato: %s\n", message);
    

}

