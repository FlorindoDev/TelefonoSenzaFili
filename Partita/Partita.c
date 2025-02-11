#include "Partita.h"


int main(int argc, char *argv[]){
    
    int pipe_read = atoi(argv[1]);
    int pipe_write = atoi(argv[2]);
    read(pipe_read, &stanza_corrente, sizeof(Stanza));
    AperturaSocket();
    printf("porta stanza: %d\n", ntohs(server_addr.sin_port));

    write(pipe_write,&(server_addr.sin_port),sizeof(server_addr.sin_port));

   
    pthread_key_create(&flag_key_game, free); 

    pthread_t thread;
    if (pthread_create(&thread, NULL, Thread_Game,NULL) != 0) {
        perror("Errore nella creazione del thread");
        return -1;
    }
    pthread_detach(thread);
    
    
    // Metti il server in ascolto
    
    if (listen(server_fd, 5) < 0) {
        perror("Errore nella listen");
        close(server_fd);
        return -1;
    }
    
    
    printf("La partita è in ascolto sulla porta %d...\n", ntohs(server_addr.sin_port));

    while (1){
    
        assignConnectionToThread(server_fd,server_addr,addr_len);
        
    }

    

    close(server_fd);

    return 0;

}

void CloseThread(int input){
    printf("prima rimozione\n");
    printf("io sono in canc %ld\n", pthread_self());
    pthread_mutex_lock(&(stanza_corrente.light));
    stanza_corrente.listaPartecipanti = removeUtenteFromThread(stanza_corrente.listaPartecipanti,pthread_self(),stanza_corrente.num_players,&stanza_corrente);
    if( stanza_corrente.listaPartecipanti == NULL){
        pthread_mutex_unlock(&(stanza_corrente.light));
        stanza_corrente.num_players--;
        pthread_exit(NULL);
    }else{
        printf("prima il numero è %d\n", stanza_corrente.num_players);
        stanza_corrente.num_players--;
        printf("dopo il numero è %d\n", stanza_corrente.num_players);
        pthread_mutex_unlock(&(stanza_corrente.light));
        if(inCoda==0)pthread_cond_signal(&cond_game);
        printf("sbloccato dopo canc\n");
        pthread_cond_signal(&cond_stato);
        printf("signal dopo canc\n");
        pthread_exit(NULL);

    }
    pthread_mutex_unlock(&(stanza_corrente.light));
}

void UnlockChat(int input){
    printf("UnlockChat\n");
}

void NoCrashHendler(int input){
    printf("NoCrashHendler\n");
    int *flag_uscita = (int *)pthread_getspecific(flag_key_game);
    *flag_uscita = 1;
    
}

void HelndlerSetter(struct sigaction * sing, __sighandler_t handler,int flag,int singal_name){
    sing->sa_handler = handler;

    sing->sa_flags = flag; // Assicura che read venga interrotta
    sigemptyset(&(sing->sa_mask));
    sigaction(singal_name, sing, NULL);
}

//fare che quando uno chiuda la connessione il thread toglie l'utente
void * Thread_GestioneNuovaConnessione(void *args){
    GestioneConnessioneArgs * arg=(GestioneConnessioneArgs *) args;
    
    struct sigaction sa;
    struct sigaction sa2;
    //struct sigaction sa3;

    HelndlerSetter(&sa, UnlockChat,0,SIGUSR1);
    HelndlerSetter(&sa2, CloseThread,0,SIGUSR2);
    //HelndlerSetter(&sa3, NoCrashHendler,0,SIGPIPE); 

    pthread_cleanup_push(cleanup_handler_connection, args);


    read(*(arg->socket), arg->buffer, sizeof(arg->buffer));

    Message msg = dividiStringa(arg->buffer,":",sizeof(arg->buffer));

    initUtente(&(arg->utente),msg.nome,msg.password,msg.lingua,msg.funzione,*(arg->socket));
    setPthread(&(arg->utente),pthread_self());

    //lavoro
    gestioneNuovaConnessione(arg->socket,arg->buffer,&(arg->utente),msg);


    //utilizzo del henadler prima della chiusura
    pthread_cleanup_pop(1);  
    pthread_exit(NULL);
}

 
void addPlayerToParty(Utente * utente){

    //pthread_mutex_lock(&mutex_stato);
    printf("prima culo\n");
    if(getStato(&stanza_corrente, &mutex_stato) == INIZIATA){
        pthread_mutex_lock(&mutex_stato);
        inCoda++;
        pthread_cond_wait(&cond_stato,&mutex_stato);
        pthread_mutex_unlock(&mutex_stato);
        printf("ho finito di aspettare\n");
    }
    printf("dopo culo\n");
    setNextInOrder(&stanza_corrente, utente);
    if(inCoda > 0)inCoda--;
    pthread_cond_signal(&cond_game);
    printf("dopo set\n");
    
    //pthread_mutex_unlock(&mutex_stato);

}

void broadcast(int * sender_socket, char * sender_messagge){

    pthread_mutex_lock(&(stanza_corrente.light));
    Utente * in_esame = stanza_corrente.listaPartecipanti;
    printf("prima del for\n");
    for(int i=0;i<stanza_corrente.num_players;i++){
        printf("nel for read\n");
        int user_socket = getUserSocket(in_esame);
        if(user_socket == -1)continue;
        if(sender_socket != NULL){
            if(user_socket != *sender_socket) send(user_socket, sender_messagge, strlen(sender_messagge), 0);
        }else{
            send(user_socket, sender_messagge, strlen(sender_messagge), 0); 
        }
        in_esame = getNext(in_esame);
        
    }
    printf("fine for read\n");
    pthread_mutex_unlock(&(stanza_corrente.light));
}


void broadcastTraduzione(int * sender_socket, char * sender_messagge,Utente * last_user){

    pthread_mutex_lock(&(stanza_corrente.light));
    Utente * in_esame = stanza_corrente.listaPartecipanti;
    char tmp_parola_brodcast[BUFFER_SIZE] = "";
    strcpy(tmp_parola_brodcast,sender_messagge);
    printf("prima del for\n");
    for(int i=0;i<stanza_corrente.num_players;i++){
        printf("nel for read\n");
        int user_socket = getUserSocket(in_esame);
        if(user_socket == -1)continue;
        if(sender_socket != NULL){
            if(user_socket != *sender_socket){ 
                strcpy(tmp_parola_brodcast,Traduzione(sender_messagge,last_user,in_esame));
                send(user_socket, tmp_parola_brodcast, strlen(tmp_parola_brodcast), 0);
                
            }
        }else{
            strcpy(tmp_parola_brodcast,Traduzione(sender_messagge,last_user,in_esame));
            send(user_socket, tmp_parola_brodcast, strlen(tmp_parola_brodcast), 0); 
        }
        in_esame = getNext(in_esame);
        
    }
    printf("fine for read\n");
    pthread_mutex_unlock(&(stanza_corrente.light));
}

// Funzione di callback per gestire i dati ricevuti
size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data) {
    size_t total_size = size * nmemb;
    strncat(data, ptr, total_size);  // Aggiunge i dati al buffer
    return total_size;
}

char tmp[4096] = "";


char* Traduzione(char* s, Utente* u1, Utente* u2){

    if(u2==NULL || u1==NULL) return s;

    CURL *curl;
    CURLcode res;
    json_t *translatedText;
    char post_data[4096] = "";
    char response_data[4096] = "";  // Buffer per memorizzare la risposta del server

    // Inizializza la libreria cURL
    printf("prima della init\n");

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    printf("dopo init\n");

    if(curl) {

        //printf("dento curl1\n");
        // Impostare l'URL di destinazione
        curl_easy_setopt(curl, CURLOPT_URL, "http://nifty_franklin:5000/translate");
        //printf("dento curl2\n");
        // Impostare i dati da inviare (equivalente a -d "q=cane&source=it&target=en")
        
        //printf("dento curl3\n");
        snprintf(post_data, sizeof(post_data), "q=%s&source=%s&target=%s", s, u1->lingua, u2->lingua);
        //printf("dento curl4\n");
        // Specificare i dati da inviare tramite POST
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
        //printf("dento curl5\n");
        // Impostare l'intestazione Content-Type
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        //printf("dento curl6\n");
        // Impostare la funzione di callback per gestire la risposta
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_data);  // Buffer per salvare i dati

        // Esegui la richiesta POST
        res = curl_easy_perform(curl);
        //printf("dento curl7\n");
        // Controlla il risultato della richiesta
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Stampa la risposta ricevuta
            printf("Response data: %s\n", response_data);

            json_error_t error;
            json_t *root = json_loads(response_data, 0, &error);

            //printf("dento curl7\n");

            // Estrazione del valore associato a "translatedText"
            translatedText = json_object_get(root, "translatedText");

            if (json_is_string(translatedText)) {
                // Stampa del valore
                printf("Il valore di translatedText è: %s\n", json_string_value(translatedText));
                strcpy(tmp, json_string_value(translatedText));
            } else {
                printf("L'oggetto 'translatedText' non è una stringa\n");
            }

            // Libera la memoria
            json_decref(root);
            //printf("dento curl9\n");
        }

        // Pulisci le intestazioni
        curl_slist_free_all(headers);

        // Pulisci la sessione cURL
        curl_easy_cleanup(curl);
    }

    // Pulisci la libreria cURL
    curl_global_cleanup();
    return tmp ? tmp : "";
}

void chatParty(int * socket, char * buffer, Utente * utente){
    
    if(socket==NULL)return;
    //aggiusta
    char buffer2[BUFFER_SIZE]="";
    strcat(buffer2,utente->nome);
    strcat(buffer2," joined in the chat");
    broadcast(socket,buffer2);

    while(getStato(&stanza_corrente, &mutex_stato) == SOSPESA){

        printf("prima read\n");
        printf("%d\n",*(socket));
        riceviRispostaSignal(*socket, buffer, BUFFER_SIZE/* ,flag_key */);
        if(getStato(&stanza_corrente, &mutex_stato) == INIZIATA)return;
        printf("dopo read\n");
    
        //aggiusta
        strcpy(buffer2,"");
        strcat(buffer2,"<");
        strcat(buffer2,utente->nome);
        strcat(buffer2,">: ");
        strcat(buffer2,buffer);

        broadcast(socket,buffer2);
        printf("dopo brodcats\n");
    }
    
}



void timerHomeMade(int time, int intervallo){
    char message[BUFFER_SIZE]="Rimmangono ";
    char tempo_rimanente[BUFFER_SIZE];
    for(int i=0;i<time;i+=intervallo){
        sprintf(tempo_rimanente, "%d", (time-i));
        strcat(message,tempo_rimanente);
        strcat(message," secondi");
        broadcast(NULL,message);
        strcpy(message, "Rimmangono ");
        sleep(intervallo);
    }
}

void * Thread_Game(void *args){

    //char bufferPartita[BUFFER_SIZE];
    int *flag = malloc(sizeof(int));
    *flag = 0;

    struct sigaction sa3;

    HelndlerSetter(&sa3, NoCrashHendler,0,SIGPIPE); 

    pthread_setspecific(flag_key_game, flag);
    while(getStato(&stanza_corrente, &mutex_stato) != FINITA){
        printf("prima del gioco\n");
        Game();
        printf("dopo del gioco\n");
    }

}

void ThreadKill(){

    pthread_mutex_lock(&(stanza_corrente.light));
    Utente * in_esame = stanza_corrente.listaPartecipanti;
    for(int i=0;i<stanza_corrente.num_players;i++){
        pthread_kill(in_esame->thread,SIGUSR1);
        in_esame = getNext(in_esame);
        if(in_esame == NULL){
            pthread_mutex_unlock(&(stanza_corrente.light));
            return;
        }

    }
    pthread_mutex_unlock(&(stanza_corrente.light));
}

void Game(){

    while(stanza_corrente.num_players < MIN_PLAYER);
    timerHomeMade(20,5);
    setIniziata(&stanza_corrente, &mutex_stato);
    ThreadKill();
    sleep(1);

    //pthread_mutex_lock(&(stanza_corrente.light));

    Utente * in_esame =  (stanza_corrente.direzione == ASC) ? stanza_corrente.listaPartecipanti : stanza_corrente.coda;
    Utente * last_user=in_esame;
    int flag_out=0;
    char parola[MAX_PAROLA]="";
    char tmp_parola[MAX_PAROLA]="";
    int user_socket = getUserSocket(in_esame);
    for(int i=0;i<stanza_corrente.num_players;i++){
        printf("numero di pleayer%d-%d porta %d\n", i,stanza_corrente.num_players,user_socket);
        int *flag_uscita = (int *)pthread_getspecific(flag_key_game);
        send(user_socket, "\nè il tuo turno:\n", strlen("\nè il tuo turno:\n"), 0);
        riceviRispostaGame(user_socket,parola,MAX_PAROLA , flag_key_game, in_esame->thread);
        if(*flag_uscita==1){
            *flag_uscita=0;
            strcpy(parola,"");
            i--;
            printf("sono GAME e mi sono bloccato\n");
            pthread_mutex_lock(&mutex_game);
            pthread_cond_wait(&cond_game,&mutex_game);
            pthread_mutex_unlock(&mutex_game);
            printf("sono GAME e mi sono ripreso\n");
            in_esame = getNextInOrder(last_user,stanza_corrente.direzione);
            flag_out=1;
            stampaLista(stanza_corrente.listaPartecipanti);
        }
        printf("prima concat\n");
        strcat(tmp_parola," ");
        strcat(tmp_parola,parola);
        printf("prima traduzione\n");
        strcpy(tmp_parola,Traduzione(tmp_parola,in_esame,getNextInOrder(in_esame,stanza_corrente.direzione)));
        last_user = in_esame;
        if(flag_out==0)in_esame = getNextInOrder(in_esame,stanza_corrente.direzione);
        flag_out=0;
        user_socket = getUserSocket(in_esame);
        if(in_esame != NULL){
            send(user_socket, tmp_parola, strlen(tmp_parola), 0);
            printf("parola finale %d: %s \n", i,tmp_parola);
        }
    }
    //pthread_mutex_unlock(&(stanza_corrente.light));
    setSospesa(&stanza_corrente, &mutex_stato);

    broadcastTraduzione(NULL,tmp_parola,last_user);
    pthread_cond_broadcast(&cond_stato);
    pthread_cond_broadcast(&cond_chat);
    printf("cond_chat brodcast\n");
    //sleep(5); //forse è vitale
    
}

void gestioneNuovaConnessione(int * socket, char * buffer, Utente* utente,Message msg){

    printf("prima del add\n");
    printUtente(utente);
    addPlayerToParty(utente);
    printf("dopo del add\n");
    while(getStato(&stanza_corrente, &mutex_stato) != FINITA){
        printf("sto nel  while\n");
        if(getStato(&stanza_corrente, &mutex_stato) == SOSPESA){
            printf("sto nel if del while\n");
            chatParty(socket,buffer,utente);
            printf("fine chat\n");
        }
        
        printf("CHAT IN ATTESSA\n");
        pthread_mutex_lock(&mutex_chat);
        pthread_cond_wait(&cond_chat,&mutex_chat);
        pthread_mutex_unlock(&mutex_chat);
        printf("mi sono riattivato\n");
    }
} 


int AperturaSocket(){   

    // Creazione della socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Impostazione dell'indirizzo della socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = 0;  // Porta 0 per far scegliere al sistema

    // Binding della socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Ottenere la porta assegnata automaticamente
    if (getsockname(server_fd, (struct sockaddr*)&server_addr, &addr_len) == -1) {
        perror("getsockname failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    return 0;
}


