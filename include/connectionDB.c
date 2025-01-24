
PGconn* connect_to_DB(){
    const char *conninfo = "host=ep-restless-voice-a48pi7t3.us-east-1.aws.neon.tech port=5432 dbname=neondb user=neondb_owner password=npg_rqCn8eUhMx1H sslmode=require options=endpoint=ep-restless-voice-a48pi7t3";
    PGconn* conn = PQconnectdb(conninfo);

     if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }
    printf("Connected to database successfully!\n");
    return conn;
}