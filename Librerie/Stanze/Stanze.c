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
    pthread_mutex_unlock(&(liststanze->light));
    

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
    
    
    strcpy(stanza->nomeStanza,nomeStanza);

    initUtente(&(stanza->proprietario), utente->nome,utente->password,utente->lingua, utente->funzione, 0);
    
    stanza->listaPartecipanti = NULL;
    stanza->coda = NULL;
    stanza->direzione = dir;
    stanza->stato=SOSPESA;
    stanza->next=NULL;
    pthread_mutex_init(&(stanza->light), NULL);

    stanza->pid_proccesso_stanza = creazioneProcessoStanza(fd);
    write(fd[1],stanza,sizeof(Stanza));
    sleep(1);
    stanza->port = returnPortaPartita(fd);
    

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

Utente * getNextInOrder(Utente * utente, enum Direzione dir){
    
    if(utente != NULL)return (dir == ASC) ? utente->next : utente->prev;
    return NULL; 
}

int existUtente(Stanza* stanza, char * nomeUtente){


    if(stanza->next == NULL){
        
        return 0;
    }
    printf("prima di while exsits\n");
    Utente *tmp = stanza->listaPartecipanti;
    printUtente(stanza->listaPartecipanti);
    while(tmp != NULL){
        printf("dopo di while exsits\n");
        printf("qui: %s-%s\n",tmp->nome,nomeUtente); //si blocca su tmp quando accediamo a nome
        if(strcmp(tmp->nome,nomeUtente) == 0){
            printf("porcodio\n");
            return 1;
        }
        tmp = tmp->next;
    }

    printf("porcodio2\n");
    return 0;
}


int insertAtFront(Stanza* stanza, Utente * utente){
    int res = -1;
    
    if(stanza->listaPartecipanti == NULL){
        stanza->proprietario = *utente;
        stanza->listaPartecipanti=utente;
        stanza->coda=utente;
        utente->next = NULL;
        utente->prev = NULL;
        res = 1;
    }else{
        if(existUtente(stanza,utente->nome) == 0){
            utente->next = stanza->listaPartecipanti;
            stanza->listaPartecipanti->prev = utente;
            utente->prev = NULL;
            stanza->listaPartecipanti = utente;
            res = 1;
        }    

    }
    
   
    return res;
}


int insertAtBack(Stanza* stanza, Utente * utente){
    int res = -1;
    
    if(stanza->listaPartecipanti == NULL){
        stanza->proprietario = *utente;
        stanza->listaPartecipanti=utente;
        stanza->coda=utente;
        utente->next = NULL;
        utente->prev = NULL;
        res = 1;
    }else{
        printf("prima di existis\n");
        if(existUtente(stanza,utente->nome) == 0){
            printf("dopo di existis\n");
            Utente * prev = stanza->listaPartecipanti;
            Utente * tmp = stanza->listaPartecipanti->next;
            while(tmp != NULL){
                printf("non finiro while insertatback\n");
                prev = tmp;
                tmp = tmp->next;
            }

            utente->prev = prev;
            prev->next = utente;
            utente->next = NULL;
            stanza->coda = utente;
            res = 1;
        }    
    }
    
    printf("fine\n");
    return res;
}



int setNextInOrder(Stanza* stanza, Utente * utente){
    printf("prima del mutex\n");
    pthread_mutex_lock(&(stanza->light));
    int res = -1;
    printf("prima del if della direzione\n");
    if(stanza->direzione == ASC){
        printf("prima del insertback\n");
        res = insertAtBack(stanza,utente);
        printf("dopo del insertback\n");
    }else{
        printf("prima del insertfront\n");
        res = insertAtFront(stanza,utente);
        printf("dopo del insertfront\n");
    }

    if(res == 1){
        
        stanza->num_players++;
        
    }
    printf("fine inserimento\n");
    pthread_mutex_unlock(&(stanza->light));
    return res;
}


enum Stato getStato(Stanza* stanza_corrente, pthread_mutex_t* mutex_stato){
    pthread_mutex_lock(mutex_stato);
    enum Stato tmp = stanza_corrente->stato;
    pthread_mutex_unlock(mutex_stato);
    return tmp; 
}

void setIniziata(Stanza* stanza_corrente, pthread_mutex_t* mutex_stato){
    pthread_mutex_lock(mutex_stato);
    stanza_corrente->stato=INIZIATA;
    pthread_mutex_unlock(mutex_stato);
}


void setSospesa(Stanza* stanza_corrente, pthread_mutex_t* mutex_stato){
    pthread_mutex_lock(mutex_stato);
    stanza_corrente->stato=SOSPESA;
    pthread_mutex_unlock(mutex_stato);
}

Utente * removeUtenteFromThread(Utente * utente, pthread_t thread, int num_players, Stanza* Stanza){

    Utente * in_esame = utente;
    while(in_esame != NULL){
        printf("for rimoziome\n");
        if(in_esame->thread==thread){
            printf("if fatto\n");
            if(in_esame->next != NULL){
                in_esame->next->prev=in_esame->prev;
                if(in_esame->prev == NULL)utente=in_esame->next;
            }else{
                if(in_esame->prev != NULL){
                    in_esame->prev->next=NULL;
                }else{
                    utente=NULL;
                }
                Stanza->coda=in_esame->prev;
            }
            if(in_esame->prev != NULL){
                in_esame->prev->next=in_esame->next;
            }else{
                if(in_esame->next != NULL){
                    in_esame->next->prev=NULL;
                    if(in_esame->prev == NULL)utente=in_esame->next;
                }else{
                    utente=NULL;
                    Stanza->coda=NULL;
                }
            }
            //free(in_esame);
        }
        in_esame = getNext(in_esame);
    }
    printf("fine rimozione\n");
    
    return utente;

    
}