# Fonti
- libpq eseguire quary: https://www.postgresql.org/docs/9.4/libpq-exec.html
- libpq Creare connessione: https://www.postgresql.org/docs/9.4/libpq-exec.html](https://www.postgresql.org/docs/9.4/libpq-connect.html#LIBPQ-CONNSTRING


# Per compilare

- `gcc -o my_program  -g main.c -I/usr/include/postgresql -lpq -lpthread`
- `gcc -o my_program -g  main.c -I/usr/include/postgresql -lpq -lpthread ; gcc -o client Client-Server/client.c`
- `valgrind --leak-check=full --track-origins=yes ./mio_programma`

# Per scaricare pg
- `sudo apt-get install --reinstall libpq-dev`

# Schema Database
![Chema Concettuale Database](https://github.com/FlorindoDev/TelefonoSenzaFili/blob/master/Database%20LSO.jpg)


