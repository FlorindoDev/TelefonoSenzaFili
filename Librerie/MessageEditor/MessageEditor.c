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

    char *token = strtok_r(copia, separatore, &saveptr);
    strcpy(msg.funzione,token);
    printf("%s\n",msg.funzione);

    //Estraggo il nome Utente passato
    token = strtok_r(NULL, separatore, &saveptr);
    strcpy(msg.nome,token);
    printf("%s\n",msg.nome);

    //Estraggo la password passata
    token = strtok_r(NULL, separatore, &saveptr);
    strcpy(msg.password,token);
    printf("%s\n",msg.password);

    token = strtok_r(NULL, separatore, &saveptr);
    strcpy(msg.lingua,token);
    

    token = strtok_r(NULL, separatore, &saveptr);
    strcpy(msg.nomeStanza,token);

    token = strtok_r(NULL, separatore, &saveptr);
    msg.direzione = atoi(token);

    free(copia);
    return msg;
}

int isLogin(Message * msg){
    return strcmp(msg->funzione,"login") ? 0 : 1;
}

int isSingUp(Message * msg){
    return strcmp(msg->funzione,"singup") ? 0 : 1;
}

int isCreate(Message * msg){
    return strcmp(msg->funzione,"create") ? 0 : 1;
}

int isShow(Message * msg){
    return strcmp(msg->funzione,"show") ? 0 : 1;
}

