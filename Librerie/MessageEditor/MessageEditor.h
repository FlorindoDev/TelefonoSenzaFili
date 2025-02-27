#ifndef MESSAGGE_EDITOR_H
#define MESSAGGE_EDITOR_H

#include <string.h>

#define MAX_SEPRATORE 1
#define MAX_LEN 50

typedef struct Message{
    char nome[MAX_LEN];
    char password[MAX_LEN];
    char lingua[MAX_LEN];
    char funzione[MAX_LEN];
    char nomeStanza[MAX_LEN];
    int direzione;
    
}Message;

Message dividiStringa(const char *input, char separatore[MAX_SEPRATORE], int dimensione);

int isLogin(Message* msg);

int isSingUp(Message * msg);

int isCreate(Message * msg);

int isShow(Message * msg);

#include "MessageEditor.c"

#endif