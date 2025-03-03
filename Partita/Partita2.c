#include "Partita2.h"



//varabbili per la connessione
int server_fd;
struct sockaddr_in server_addr;
socklen_t addr_len = sizeof(server_addr);

Stanza stanza_corrente;

int players_waiting=0;

int AperturaSocket();

int main(int argc, char *argv[]){

    // Nel main, prima di altre operazioni:
    signal(SIGPIPE, SIG_IGN);
    
    int pipe_read = atoi(argv[1]);
    int pipe_write = atoi(argv[2]);
    read(pipe_read, &stanza_corrente, sizeof(Stanza));
    AperturaSocket();
    printf("porta stanza: %d\n", ntohs(server_addr.sin_port));

    write(pipe_write,&(server_addr.sin_port),sizeof(server_addr.sin_port));


    //Creo il thread della partita
    pthread_t thread;
    if (pthread_create(&thread, NULL, thread_Game,NULL) != 0) {
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

void * thread_Game(void *args){
    while(getStato(&stanza_corrente, &mutex_stato) != FINITA){
        Game();
    }
}


void Game(){
    while(stanza_corrente.num_players < MIN_PLAYER);
    timerHomeMade(5,2);
    //Logica del gioco
    setIniziata(&stanza_corrente, &mutex_stato);
    printf("Gioco in corso\n");

    //sleep(5);//simulo la partita
    
    propagateGamePhrase(); //propaga il messaggio tra tutti i giocatori

    setSospesa(&stanza_corrente, &mutex_stato);
    addAllPlayersWaiting();
    riprendiChat();
}



void propagateGamePhrase(){

    printList();
    
    Utente *in_esame = (stanza_corrente.direzione == ASC) ? stanza_corrente.listaPartecipanti : stanza_corrente.coda;
    char phrase[GAME_PHRASE_MAX_SIZE] = "";
    char user_contribute[BUFFER_SIZE] = "";

    Utente * last_user;
    
    int remaining_players = stanza_corrente.num_players;
    int i = 0;

    printf("Direzione della stanza: %s\n",(stanza_corrente.direzione == ASC) ? "ASC" : "DESC");
    
    while (i < remaining_players && in_esame != NULL) {
        printf("\niterazione numero %d reamining_players: %d\n",i,remaining_players);
        int user_socket = getUserSocket(in_esame);
        
        // Verifica se il socket è ancora valido
        if (user_socket == -1 || !isSocketConnected(user_socket)) {
            printf("Il giocatore %s si è disconnesso, passo al prossimo\n", in_esame->nome);
            
            // Salva un riferimento temporaneo al prossimo giocatore prima di rimuovere in_esame

            int aggiunto=aggiungiProssimoDallaCoda();

            sleep(2); //aspetto che venga aggiunto

            Utente *next_temp = getNextInOrder(in_esame, stanza_corrente.direzione);
            
            rimuoviGiocatore(in_esame);
            
            if(aggiunto){
                // Un nuovo giocatore è stato aggiunto alla lista, continuiamo da next_temp
                in_esame = next_temp;
            } else {
                // Nessun giocatore aggiunto dalla coda
                remaining_players--; 

                if(next_temp != NULL) last_user = in_esame;
                in_esame = next_temp;
            }
        }
        else {
            // Invia messaggio al giocatore e attendi risposta
            int send_result1 = 0;
            int send_result2 = 0;
            
            // Se phrase è vuoto (all'inizio o dopo un reset), invia solo il messaggio del turno
            if (strlen(phrase) > 0) {
                send_result1 = send(user_socket, phrase, strlen(phrase), 0);
                if (send_result1 < 0) {
                    printf("Errore nell'invio della frase al giocatore %s: %s\n", in_esame->nome, strerror(errno));
                }
            }
            
            send_result2 = send(user_socket, "\nè il tuo turno:\n", strlen("\nè il tuo turno:\n"), 0);
            if (send_result2 < 0) {
                printf("Errore nell'invio del messaggio di turno al giocatore %s: %s\n", in_esame->nome, strerror(errno));
            }
            
            // Controlla se c'è stato un errore in uno dei due invii
            if ((strlen(phrase) > 0 && send_result1 < 0) || send_result2 < 0) {
                printf("Errore nell'invio dei messaggi al giocatore %s, passo al prossimo\n", in_esame->nome);
                
                // Salva un riferimento temporaneo al prossimo giocatore prima di rimuovere in_esame
                Utente *next_temp = getNextInOrder(in_esame, stanza_corrente.direzione);
                
                rimuoviGiocatore(in_esame);
                remaining_players--;

                if(next_temp != NULL) last_user = in_esame;
                in_esame = next_temp;
            }
            else if (!riceviRispostaConTimeout(user_socket, user_contribute, GAME_PHRASE_MAX_SIZE, 30)) {
                // Timeout o errore di ricezione
                printf("Timeout o errore per il giocatore %s, passo al prossimo\n", in_esame->nome);
                
                
                
                
                if(!aggiungiProssimoDallaCoda()){
                    remaining_players--; //se non c'era nessuno in coda
                }

                sleep(2); //aspetto che venga aggiunto

                // Salva un riferimento temporaneo al prossimo giocatore prima di rimuovere in_esame
                Utente *next_temp = getNextInOrder(in_esame, stanza_corrente.direzione);

                rimuoviGiocatore(in_esame);
                
                if(next_temp != NULL) last_user = in_esame;
                in_esame = next_temp;
                printf("il nome del prossimo utente è %s\n\n",in_esame->nome);
                printList();
            }
            else {

                Utente *next_temp = getNextInOrder(in_esame, stanza_corrente.direzione);
                // Risposta ricevuta con successo
                printf("Risposta ricevuta con successo dal giocatore %s: '%s'\n", in_esame->nome, user_contribute);
                strcat(phrase, " ");
                strcat(phrase, user_contribute); 
                strcpy(phrase,Traduzione(phrase,in_esame,next_temp));
                strcpy(user_contribute, "");
                
                // Passa al prossimo giocatore
                if(next_temp != NULL) last_user = in_esame;
                in_esame = next_temp;
            
                i++; // Incrementa solo se un giocatore ha risposto con successo
            }
        }
        
        printList();
    }
    
    // Aggiorna il numero di giocatori nella stanza
    stanza_corrente.num_players = remaining_players;

    printf("ALLA FINE DEL GIOCO LA LISTA è:\n");
    printList();
    
    // Se c'è almeno una frase, fai il broadcast
    if (strlen(phrase) > 0) {
        //broadcast(NULL, phrase);
        broadcastTraduzione(NULL,phrase,last_user);
    }
    else {
        broadcast(NULL, "Nessun giocatore ha contribuito alla frase.");
    }
}

void printList(){
    Utente* in_esame=stanza_corrente.listaPartecipanti;

    char lista[1000]="";
    char tmp[20]="";

    while(in_esame!=NULL){
        strcat(tmp,in_esame->nome);
        strcat(tmp,"->");
        strcat(lista,tmp);

        in_esame=in_esame->next;
        strcpy(tmp,"");
    }

    printf("\n\n---LISTA--- %s\n\n",lista);
}

int isSocketConnected(int sock){
    char buff[1];
    int result = recv(sock, buff, 1, MSG_PEEK | MSG_DONTWAIT);
    
    if (result == 0) {
        // La connessione è stata chiusa correttamente
        return 0;
    } else if (result < 0) {
        // Controlla se è un errore temporaneo
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Non ci sono dati ma la connessione è viva
            return 1;
        } else {
            // Errore di connessione
            return 0;
        }
    }
    // Ci sono dati da leggere, la connessione è attiva
    return 1;
}

void rimuoviGiocatore(Utente * utente){
    pthread_mutex_lock(&(stanza_corrente.light));

    Utente *in_esame=stanza_corrente.listaPartecipanti;
    Utente *prev=NULL;

    while(in_esame!=NULL){
        if(in_esame->nome == utente->nome){ //trovato

            if(in_esame->prev==NULL){ //se sono la testa
                if(in_esame->next==NULL){ //e sono anche la coda
                    in_esame=NULL;
                    stanza_corrente.listaPartecipanti=NULL;
                    stanza_corrente.coda=NULL;
                    //ero l'ultimo giocatore rimasto quindi termina il gioco
                }else{
                    //sono solo la testa quindi prendi il prossimo e fallo diventare la testa
                    Utente *next=in_esame->next;
                    next->prev=NULL;
                    in_esame=NULL;
                    stanza_corrente.listaPartecipanti=next;
                    

                }
            }else{

                if(in_esame->next==NULL){ 
                    //se sono la coda prendi il mio precedente e imposta la coda 
                    prev->next=NULL;
                    stanza_corrente.coda=prev;

                }else{

                    //sto in mezzo e devo fare i collegamenti giusti
                    prev->next=in_esame->next;
                    in_esame->next->prev=prev;

                }
            }

            stanza_corrente.num_players--;

            // Chiudi il socket dell'utente
            int user_socket = getUserSocket(utente);
            if (user_socket != -1) {
                close(user_socket);
            }
            
            printf("giocatore eliminato num: %d\n",stanza_corrente.num_players);
            pthread_mutex_unlock(&(stanza_corrente.light));  // Sblocca prima di uscire
            return;

        }

        prev=in_esame;
        in_esame=in_esame->next; //vai avanti di uno step

    }

    pthread_mutex_unlock(&(stanza_corrente.light));
}

int riceviRispostaConTimeout(int socket, char *buffer, size_t size, int timeout_seconds) {
   
    time_t start_time = time(NULL);
    struct pollfd fd;
    fd.fd = socket;
    fd.events = POLLIN;
    
    // Buffer per i controlli periodici
    char check_buffer[1];
    
    // Loop fino al timeout totale
    while (time(NULL) - start_time < timeout_seconds) {
        // Poll con un timeout breve (es. 2 secondi)
        int ret = poll(&fd, 1, 2000);
        
        if (ret < 0) {
            // Errore nella poll
            return 0;
        } 
        else if (ret == 0) {
            // Nessun dato disponibile, verifica se il socket è ancora connesso
            if (!isSocketConnected(socket)) {
                // Il client si è disconnesso
                return 0;
            }
            // Altrimenti continua ad aspettare
            continue;
        }
        
        // Ci sono dati da leggere
        if (fd.revents & POLLIN) {
            ssize_t bytes = recv(socket, buffer, size - 1, 0);
            if (bytes <= 0) {
                // Il client si è disconnesso
                return 0;
            }
            buffer[bytes] = '\0';
            return 1; // Successo
        }
    }
    
    // Timeout raggiunto
    return 0;
    
}

void * Thread_GestioneNuovaConnessione(void *args){

    GestioneConnessioneArgs * arg=(GestioneConnessioneArgs *) args;
    
   
    pthread_cleanup_push(cleanup_handler_connection, args);

    //Legge info del utente che si sta connetento
    read(*(arg->socket), arg->buffer, sizeof(arg->buffer));

    //buffer è il buffer dell utente appena connesso
    Message msg = dividiStringa(arg->buffer,":",sizeof(arg->buffer));

    initUtente(&(arg->utente),msg.nome,msg.password,msg.lingua,msg.funzione,*(arg->socket));
    
    //assegna al utente il thread che lo gestirà 
    setPthread(&(arg->utente),pthread_self());

    //lavoro
    ChooseAction(arg->socket,arg->buffer,&(arg->utente));

    

    //utilizzo del henadler prima della chiusura
    pthread_cleanup_pop(1);  
    pthread_exit(NULL);
}

void entroInAttesa(){
    pthread_mutex_lock(&mutex_stato);
    players_waiting++;
    pthread_cond_wait(&cond_stato,&mutex_stato);
    pthread_mutex_unlock(&mutex_stato);
}

int aggiungiProssimoDallaCoda(){
    pthread_mutex_lock(&mutex_stato);
    if(players_waiting>0){
        pthread_cond_signal(&cond_stato);
        players_waiting--;

        pthread_mutex_unlock(&mutex_stato);
        return 1;
    }else{
        pthread_mutex_unlock(&mutex_stato);
        return 0;
    }
    
}

void addAllPlayersWaiting(){
    pthread_mutex_lock(&mutex_stato);
    pthread_cond_broadcast(&cond_stato);
    pthread_mutex_unlock(&mutex_stato); 
}


void chatPause(){
    pthread_mutex_lock(&mutex_chat);
    pthread_cond_wait(&cond_chat,&mutex_chat);
    pthread_mutex_unlock(&mutex_chat);
}

void riprendiChat(){
    pthread_mutex_lock(&mutex_chat);
    pthread_cond_broadcast(&cond_chat);  // Sveglia i thread della chat
    pthread_mutex_unlock(&mutex_chat);  
}

void addPlayer(Utente * utente){

    //se la partita è già iniziata cambia dinamicamente il modo di inserimento
    if(getStato(&stanza_corrente, &mutex_stato) == INIZIATA){
        insertInGame(&stanza_corrente,utente);
        
    }else{
        insertAtBackSafe(&stanza_corrente,utente); //se non è iniziata inserisci sempre in coda
    }

    printf("\n\nGIOCATORE AGGIUNTO %s\n\n",utente->nome);

}

void addNameToMessage(char * message, Utente * utente){
    strcat(message,"<");
    strcat(message,utente->nome);
    strcat(message,">: ");
}


void enterInChat(int * socket, char * buffer, Utente * utente){

 
    struct pollfd fds[1];
    fds[0].fd = *socket;
    fds[0].events = POLLIN;  // Monitoriamo la possibilità di lettura

    char message[BUFFER_SIZE] = "";

    while (getStato(&stanza_corrente,&mutex_stato) != INIZIATA) {
        int ret = poll(fds, 1, TIMEOUT);

        if (ret == -1) {
            perror("poll");
            break;
        } else if (ret == 0) {
            // Timeout: nessun dato ricevuto, controllo flag per cambiare modalità
            printf("Timeout, controllo stato...\n");
            continue;
        }

        if (fds[0].revents & POLLIN) {
           
            ssize_t bytes = recv(*socket, buffer, BUFFER_SIZE, 0);
            if (bytes <= 0) { 
                // Se recv() ritorna 0, il client si è disconnesso
                printf("Il client %s si è disconnesso dalla chat.\n", utente->nome);
                rimuoviGiocatore(utente);
                close(*socket);
                return; 
            }
            buffer[bytes] = '\0';
            strcpy(message, "");
            addNameToMessage(message, utente);
            strcat(message, buffer);
            printf("stringa broadcast: %s", buffer);
            broadcast(socket, message);
        }
    }

    
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



void broadcast(int * sender_socket, char * sender_messagge){

    pthread_mutex_lock(&(stanza_corrente.light));
    Utente * in_esame = stanza_corrente.listaPartecipanti;

    for(int i=0;i<stanza_corrente.num_players;i++){

        int user_socket = getUserSocket(in_esame);
        if(user_socket == -1)continue;
        

        if(sender_socket != NULL){
            if(user_socket != *sender_socket) send(user_socket, sender_messagge, strlen(sender_messagge), 0);
        }else{
            send(user_socket, sender_messagge, strlen(sender_messagge), 0); 
        }
        in_esame = getNext(in_esame);
        
    }

    pthread_mutex_unlock(&(stanza_corrente.light));
}

void ChooseAction(int * socket, char * buffer, Utente * utente){

    if(getStato(&stanza_corrente, &mutex_stato) == INIZIATA){
        entroInAttesa();
        printf("sono uscito dall attesaaaa utente:%s\n",utente->nome);
    }
    addPlayer(utente);

    while(getStato(&stanza_corrente,&mutex_stato) != FINITA){
        enterInChat(socket, buffer,utente);
        chatPause();
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

        
        // Impostare l'URL di destinazione
        curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.0.149:5000/translate");
        
        // Impostare i dati da inviare (equivalente a -d "q=cane&source=it&target=en")
        
        
        snprintf(post_data, sizeof(post_data), "q=%s&source=%s&target=%s", s, u1->lingua, u2->lingua);
       
        // Specificare i dati da inviare tramite POST
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
       
        // Impostare l'intestazione Content-Type
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
       
        // Impostare la funzione di callback per gestire la risposta
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_data);  // Buffer per salvare i dati

        // Esegui la richiesta POST
        res = curl_easy_perform(curl);
        
        // Controlla il risultato della richiesta

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Stampa la risposta ricevuta
            printf("Response data: %s\n", response_data);

            json_error_t error;
            json_t *root = json_loads(response_data, 0, &error);


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