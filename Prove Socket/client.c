#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int login();
int signUp();
void homeShow();
void cleanSocketBuffer(int sock, char buffer[BUFFER_SIZE], int size);


int sock = 0;
struct sockaddr_in serv_addr;
char buffer[BUFFER_SIZE] = {0};


int main() {
    int scelta;
    int risultato;

    do {
        printf("\n=== Menu ===\n");
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
                printf("Uscita dal programma.\n");
                break;
            default:
                printf("Opzione non valida. Riprova.\n");
                break;
        }
    } while (scelta != 3);

    return 0;
}

int login(){
    char nome[50], password[50];
    
    // Chiedere il nome
    printf("Inserisci il tuo nome: ");
    scanf("%49s", nome); // Limitiamo l'input a 49 caratteri per evitare buffer overflow

    // Chiedere la password
    printf("Inserisci la tua password: ");
    scanf("%49s", password); // Limitiamo l'input a 49 caratteri


    char message[100];
    strcpy(message,"login:");
    strcat(message,nome);
    strcat(message,":");
    strcat(message,password);
    strcat(message,":");
    

    // Mostrare i dati inseriti
    printf("\n--- Dati Inseriti ---\n");
    printf("Nome: %s\n", nome);
    printf("Password: %s\n", password);

    printf("\n %s \n",message);

    // Creazione della socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Errore nella creazione della socket");
        return -1;
    }else{
        printf("Socket creata\n");
    }
    
    // Configurazione dell'indirizzo del server
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");;
    
    // Connessione al server
    if ((connect(sock, (struct sockaddr*)&serv_addr,sizeof(serv_addr))) < 0) {
        perror("Errore nella connessione");
        close(sock);
        return -1;
    }else{
        printf("Connessione creata\n");
    }


    // Invia il messaggio al server
    send(sock, message, strlen(message), 0);
    printf("Messaggio inviato: %s\n", message);

    // Riceve la risposta dal server
    int charPassati = read(sock, buffer, BUFFER_SIZE);
    if (charPassati < BUFFER_SIZE){
        buffer[charPassati]='\0';
    }
    
    printf("Risposta dal server: %s\n", buffer);
    
    if(strcmp("-1",buffer) == 0){
        return -1;
    }else{
        return 1;

    }
    
    /*
    if(strcmp("Login",buffer) == 0){

        cleanSocketBuffer(sock, buffer, BUFFER_SIZE);
        close(sock);
        return 1;
    }

    // Chiudi la socket
    cleanSocketBuffer(sock, buffer, BUFFER_SIZE);

    */
 
    close(sock);
    return 0;

}

int signUp(){
    char nome[50], password[50], lingua[20];

    // Chiedere il nome
    printf("Inserisci il tuo nome: ");
    scanf("%49s", nome); // Limitiamo l'input a 49 caratteri per evitare buffer overflow

    // Chiedere la password
    printf("Inserisci la tua password: ");
    scanf("%49s", password); // Limitiamo l'input a 49 caratteri

    // Chiedere la lingua
    printf("Inserisci la tua lingua preferita: ");
    scanf("%19s", lingua); // Limitiamo l'input a 19 caratteri

    char message[100];
    strcpy(message,"signup:");
    strcat(message,nome);
    strcat(message,":");
    strcat(message,password);
    strcat(message,":");
    strcat(message,lingua);
    strcat(message,":");

    // Mostrare i dati inseriti
    printf("\n--- Dati Inseriti ---\n");
    printf("Nome: %s\n", nome);
    printf("Password: %s\n", password);
    printf("Lingua: %s\n", lingua);

    // Creazione della socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Errore nella creazione della socket");
        return -1;
    }else{
        printf("Socket creata\n");
    }
    
    // Configurazione dell'indirizzo del server
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");;
    
    // Connessione al server
    if ((connect(sock, (struct sockaddr*)&serv_addr,sizeof(serv_addr))) < 0) {
        perror("Errore nella connessione");
        close(sock);
        return -1;
    }else{
        printf("Connessione creata\n");
    }


    // Invia il messaggio al server
    send(sock, message, strlen(message), 0);
    printf("Messaggio inviato: %s\n", message);
    
    // Riceve la risposta dal server
    int charPassati = read(sock, buffer, BUFFER_SIZE);
    if (charPassati < BUFFER_SIZE){
        buffer[charPassati]='\0';
    }
    
    
    printf("Risposta dal server: %s\n", buffer);
    
    if(strcmp("-1",buffer) == 0){
        return -1;
    }else{
        return 1;

    }

    
    
    /*
    if(strcmp("Registrato",buffer) == 0){

        cleanSocketBuffer(sock, buffer, BUFFER_SIZE);
        close(sock);
        return 1;
    }
    
    // Chiudi la socket
    cleanSocketBuffer(sock, buffer, BUFFER_SIZE);
    */

    close(sock);
    return 0;
    


}

void homeShow(){
 


}

void cleanSocketBuffer(int sock, char buffer[BUFFER_SIZE], int size){
    int nread;
    while((nread = read(sock, buffer, size-1)) > 0) {
        buffer[nread]='\0';    // explicit null termination: updated based on comments
        printf("%s\n",buffer); // print the current receive buffer with a newline
        fflush(stdout);         // make sure everything makes it to the output
        buffer[0]='\0';        // clear the buffer : I am 99% sure this is not needed now
    }

}