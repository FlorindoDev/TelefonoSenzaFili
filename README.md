# Fonti
- libpq eseguire quary: https://www.postgresql.org/docs/9.4/libpq-exec.html
- libpq Creare connessione: https://www.postgresql.org/docs/9.4/libpq-exec.html](https://www.postgresql.org/docs/9.4/libpq-connect.html#LIBPQ-CONNSTRING


# Per compilare

- `gcc -o my_program main.c -I/usr/include/postgresql -lpq`
- `gcc -o my_program main.c -I/usr/include/postgresql -lpq -lpthread ; gcc -o client Client-Server/client.c`

# Per scaricare pg
- `sudo apt-get install --reinstall libpq-dev`

# Schema Database
![Chema Concettuale Database](https://github.com/FlorindoDev/TelefonoSenzaFili/blob/master/Database%20LSO.jpg)


