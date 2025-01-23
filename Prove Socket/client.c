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
    read(sock, buffer, BUFFER_SIZE);
    printf("Risposta dal server: %s\n", buffer);

    // Chiudi la socket
    close(sock);
    

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
    read(sock, buffer, BUFFER_SIZE);
    printf("Risposta dal server: %s\n", buffer);

    // Chiudi la socket
    close(sock);

    


}

void homeShow(){
 


}

