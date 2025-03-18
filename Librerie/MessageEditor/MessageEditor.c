#include <string.h>


Message dividiStringa(const char *input, char separatore[MAX_SEPRATORE], int dimensione){

    //il messaggio è fatto comando:nome:pass:lingua:nomestanza:direzione
    Message msg;
    char * copia = (char *)malloc(dimensione);
    char *saveptr;
    strncpy(copia, input, dimensione); // Copia la stringa ricevuta per non modificarla direttamente

    //strtok_r thread safe
    char *token = strtok_r(copia, separatore, &saveptr);
    strcpy(msg.funzione,token);


    //Estraggo il nome Utente passato
    token = strtok_r(NULL, separatore, &saveptr);
    strcpy(msg.nome,token);


    //Estraggo la password passata
    token = strtok_r(NULL, separatore, &saveptr);
    strcpy(msg.password,token);


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

