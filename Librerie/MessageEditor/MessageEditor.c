#include <string.h>

/*
void creaComando(Uchar* message , char* funzione){

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
    
    
}*/


Message dividiStringa(const char *input, char separatore[MAX_SEPRATORE], int dimensione){

    Message msg;
    char * copia = (char *)malloc(dimensione);
    char *saveptr;
    strncpy(copia, input, dimensione); // Copia la stringa ricevuta per non modificarla direttamente

    char *token = strtok_r(copia, ":", &saveptr);
    strcpy(msg.funzione,token);
    printf("%s\n",msg.funzione);

    //Estraggo il nome Utente passato
    token = strtok_r(NULL, ":", &saveptr);
    strcpy(msg.nome,token);
    printf("%s\n",msg.nome);

    //Estraggo la password passata
    token = strtok_r(NULL, ":", &saveptr);
    strcpy(msg.password,token);
    printf("%s\n",msg.password);

    token = strtok_r(NULL, ":", &saveptr);
    strcpy(msg.lingua,token);


    token = strtok_r(NULL, ":", &saveptr);
    strcpy(msg.nomeStanza,token);

    token = strtok_r(NULL, ":", &saveptr);
    msg.direzione = atoi(token);

    free(copia);
    return msg;
}

