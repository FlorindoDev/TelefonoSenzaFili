#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#include "../Librerie/Stanze/Stanze.h"
#include "../Librerie/Utente.h"

#define PORT 8080
#define BUFFER_SIZE 1024



//Menu dopo essere loggato
void homeShow();

//Funzioni richieste dall'utente
int login();
int signUp();
int entraStanzaGioco();
int creaStanzaGioco();

//Funzione creazione del messaggio ben formattato
void creaComando(char*  , char* );

//Funzioni per gestione socket
int creaSocket();
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

    int successo = creaSocket();

    if(successo != -1){
        
        // Invia il messaggio al server
        mandaMessaggio(message);
       
        // Riceve la risposta dal server
        riceviRisposta();
        
        chiudiSocket();
    
        if(strcmp("-1",buffer) == 0){
            return -1;
        }else{
            return 1;

        }
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

    int successo = creaSocket();
    
    if (successo != -1){

        // Invia il messaggio al server
        mandaMessaggio(message);
    
        riceviRisposta();
        
        // Riceve la risposta dal server
        chiudiSocket();

        if(strcmp("-1",buffer) == 0){
            return -1;
        }else{
            return 1;

        }
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
                risultato = entraStanzaGioco();
                

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

int entraStanzaGioco(){

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

    char message[BUFFER_SIZE];
    creaComando(message,"create");

    int successo = creaSocket();

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


int creaSocket(){
    
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
    serv_addr.sin_port = htons(PORT);
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


    strcpy(message,funzione);
    strcat(message,":");
    strcat(message,utente.nome);
    strcat(message,":");
    strcat(message,utente.password);
    strcat(message,":");

    strcmp(utente.lingua,"") ? strcat(message,utente.lingua) : strcat(message," ");
    
    strcat(message,":");
    strcat(message,stanza.nomeStanza);
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

