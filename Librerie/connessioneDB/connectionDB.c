
PGconn* connect_to_DB(){
    const char *conninfo = "host=192.168.92.27 port=5433 dbname=mydatabase user=db password=db";
    PGconn* conn = PQconnectdb(conninfo);

     if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }
    printf("Connected to database successfully!\n");
    return conn;
}