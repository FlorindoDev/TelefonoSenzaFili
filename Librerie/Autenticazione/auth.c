
int login(PGconn* conn,Utente* utente, char * ling){
    
    char query[512];
    snprintf(query, sizeof(query), "SELECT * FROM utente WHERE nome = '%s' AND password_utente = '%s'", utente->nome, utente->password);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error executing query: %s", PQerrorMessage(conn));
        PQclear(res);
        return 0; // Errore nell'esecuzione della query
    }

    int rows = PQntuples(res);
    if (rows == 1) {
        printf("User authenticated successfully!\n");

        //Prende la lingua
        strcpy(utente->lingua,PQgetvalue(res,0,2));
        strcpy(ling,PQgetvalue(res,0,2));
        
        PQclear(res);
        return 1; // Login riuscito
    } else {
        printf("Invalid username or password.\n");
        PQclear(res);
        return 0; // Login fallito
    }

}

int register_user(PGconn* conn,Utente* utente){
    
    char query[512];

    // Verifica se l'utente esiste già
    snprintf(query, sizeof(query), "SELECT nome FROM Utente WHERE nome = '%s'", utente->nome);
    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error checking user existence: %s", PQerrorMessage(conn));
        PQclear(res);
        return 0; // Errore nella query
    }

    int rows = PQntuples(res);
    PQclear(res);

    if (rows > 0) {
        printf("User already exists.\n");
        return 0; // Utente già esistente
    }

    
    
    // Se l'utente non esiste, esegui l'inserimento
    snprintf(query, sizeof(query),"INSERT INTO Utente (nome,Password_Utente,Lingua) VALUES ('%s', '%s', '%s')", utente->nome, utente->password,utente->lingua);
    res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error registering user: %s", PQerrorMessage(conn));
        PQclear(res);
        return 0; // Errore nell'inserimento
    }

    printf("User registered successfully!\n");
    PQclear(res);
    return 1; // Registrazione riuscita
}
