#include <string.h>
#include "Stanze.h"

void inserisciStanza(ListStanze* liststanze, Stanza* new){
    
    pthread_mutex_lock(&(liststanze->light));
    new->stato = SOSPESA;
    pthread_mutex_init(&(new->light), NULL);
    if(liststanze->next == NULL){
        new->next=NULL;
        liststanze->next = new;
    }else{
        new->next=liststanze->next;
        liststanze->next=new;
    }
    pthread_mutex_unlock(&(liststanze->light));\
    

}


int existStanza(ListStanze* liststanze, char * nomeStanza){

    pthread_mutex_lock(&(liststanze->light));
    if(liststanze->next == NULL){
        pthread_mutex_unlock(&(liststanze->light));
        return 0;
    }
    Stanza *tmp = liststanze->next;
    while(tmp != NULL){
        if(strcmp(tmp->nomeStanza,nomeStanza) == 0){
            pthread_mutex_unlock(&(liststanze->light));
            return 1;
        }
        tmp = tmp->next;
    }

    pthread_mutex_unlock(&(liststanze->light));
    return 0;
}

ListStanze* initTesta(){
    ListStanze* listStanze = (ListStanze *)malloc(sizeof(ListStanze));
    listStanze->next = NULL; 
    if (pthread_mutex_init(&(listStanze->light), NULL) != 0) {
        perror("Errore durante l'inizializzazione del mutex");
        exit(-1);
    }
    return listStanze;
}

char*  showStanze(ListStanze* liststanze){
    pthread_mutex_lock(&(liststanze->light));
   
    char message[1024];
    char * res = (char *) malloc((1024)*sizeof(char));
 
    Stanza * tmp = liststanze->next;
    
    if(tmp != NULL)
        strcpy(res,"");
    else    
        strcpy(res,"-1");

    while(tmp != NULL){
        sprintf(message, "%s (%u):",tmp->nomeStanza,tmp->port);
        strcat(res,message);

        tmp = tmp->next;
    }

    pthread_mutex_unlock(&(liststanze->light));
    return res;

}


int removeStanza(ListStanze* liststanze, char* nomeStanza){
    
    pthread_mutex_lock(&(liststanze->light));
    if(liststanze->next == NULL){
        pthread_mutex_unlock(&(liststanze->light));
        return 0;
    }

    Stanza *tmp = liststanze->next;
    Stanza *pre = NULL;
    
    while(tmp != NULL){
        if(pre == NULL && strcmp(tmp->nomeStanza,nomeStanza)==0){
            liststanze->next = tmp->next;
            free(tmp);
            pthread_mutex_unlock(&(liststanze->light));
            return 1;
        }else if(strcmp(tmp->nomeStanza,nomeStanza)==0){
            pre->next=tmp->next;
            free(tmp);
            pthread_mutex_unlock(&(liststanze->light));
            return 1;
        }
        pre = tmp;
        tmp=tmp->next;
    }
    pthread_mutex_unlock(&(liststanze->light));
    return 0;

}

ListStanze* freeStanze(ListStanze* liststanze){
    if(liststanze->next == NULL){
        return NULL;
    }
    Stanza *tmp = liststanze->next;
    Stanza *pre = NULL;
    while(tmp != NULL){
        pre = tmp;
        tmp = tmp->next;
        free(pre);
    }
    pthread_mutex_destroy(&(liststanze->light));
    free(liststanze);
    return NULL;
}

void printStanze(ListStanze* liststanze){

    pthread_mutex_lock(&(liststanze->light));
    Stanza * s = liststanze->next;
    printf("[LISTA]");
    while(s != NULL){
        printf(" ->%s(%s) ",s->nomeStanza,s->proprietario.nome);
        s = s->next;
    }
    
    printf(" -|\n");
    pthread_mutex_unlock(&(liststanze->light));
}

void initStanza(Stanza * stanza, Utente* utente, char * nomeStanza, enum Direzione dir,int* fd){
    
    strcpy(stanza->proprietario.nome,utente->nome);
    strcpy(stanza->proprietario.password,utente->password);
    strcpy(stanza->proprietario.lingua,utente->lingua);
    strcpy(stanza->nomeStanza,nomeStanza);
    stanza->listaPartecipanti = &(stanza->proprietario);
    stanza->direzione = dir;


    stanza->pid_proccesso_stanza = creazioneProcessoStanza(fd);
    write(fd[1],stanza,sizeof(Stanza));
    sleep(1/20);
    stanza->port = returnPortaPartita(fd);
    

}


Utente * getNext(Utente * utente){
    if(utente != NULL)return utente->next;
}

pid_t creazioneProcessoStanza(int* fd){
    pid_t pid = fork();
    if(pid == 0){
        char fd_str[32];
        char fd_str2[32];
        sprintf(fd_str, "%d", fd[0]);
        sprintf(fd_str2, "%d", fd[1]);
        printf("Avvio partita...\n");
        execlp(PATH_EXE_PARTITA, "PartitaEXE", fd_str, fd_str2 ,NULL);
    }
    return pid;
}

int returnPortaPartita(int* fd){

    struct sockaddr_in server_addr_stanza;
    memset(&server_addr_stanza, 0, sizeof(server_addr_stanza));
    
    //legge dal file la porta in cui è un ascolto la stanza creata tramite la PIPE
    read(fd[0],&(server_addr_stanza.sin_port),sizeof(server_addr_stanza.sin_port));

    return ntohs(server_addr_stanza.sin_port);
}

Utente * getNextInOrder(ListStanze* liststanze, Utente * utente){
    Utente * prossimo = NULL;
    pthread_mutex_lock(&(liststanze->light));
    if(utente != NULL){
        Utente * tmp = liststanze->next->listaPartecipanti;
        while(strcmp(tmp->nome,utente->nome)!=0){
            if(liststanze->next->direzione == ASC){
                tmp = tmp->next;
            }else{
                tmp = tmp->prev;
            }
        }

        if(liststanze->next->direzione == ASC){
            prossimo = tmp->next;
        }else{
            prossimo = tmp->prev;
        }
    }
    pthread_mutex_unlock(&(liststanze->light));
    return prossimo;

}

int existUtente(ListStanze* liststanze, char * nomeUtente){

    pthread_mutex_lock(&(liststanze->light));
    if(liststanze->next == NULL){
        pthread_mutex_unlock(&(liststanze->light));
        return 0;
    }
    Utente *tmp = liststanze->next->listaPartecipanti;
    while(tmp != NULL){
        if(strcmp(tmp->nome,nomeUtente) == 0){
            pthread_mutex_unlock(&(liststanze->light));
            return 1;
        }
        tmp = tmp->next;
    }

    pthread_mutex_unlock(&(liststanze->light));
    return 0;
}

int setNextInOrder(ListStanze* liststanze, Utente * utente){
    int res = -1;
    Stanza * stanza = liststanze->next;
    pthread_mutex_lock(&(liststanze->light));
    if(utente != NULL){
        if(stanza->listaPartecipanti == NULL){
            stanza->proprietario = *utente;
            stanza->listaPartecipanti=utente;
            res = 1;
        }else{
            if (existUtente(liststanze,utente->nome) == 0){
                Utente * prev = stanza->listaPartecipanti;
                Utente * tmp = stanza->listaPartecipanti->next;
                while(tmp != NULL){
                    if(liststanze->next->direzione == ASC){
                        tmp = tmp->next;
                    }else{
                        tmp = tmp->prev;
                    }
                    prev = tmp;
                }

                if(liststanze->next->direzione == ASC){
                    prev->next = utente;
                }else{
                    prev->prev = utente;
                }
                res = 1;

            }
            
            
        }

    }


    pthread_mutex_unlock(&(liststanze->light));
    return res;
}