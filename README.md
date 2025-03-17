# This code 
![This Code](https://i.pinimg.com/originals/98/fd/c7/98fdc7205a7209d34d95cdd5bce345c5.gif)

# Fonti
- libpq eseguire quary: https://www.postgresql.org/docs/9.4/libpq-exec.html
- libpq Creare connessione: https://www.postgresql.org/docs/9.4/libpq-exec.html](https://www.postgresql.org/docs/9.4/libpq-connect.html#LIBPQ-CONNSTRING


# Per compilare

- `gcc -o my_program  -g main.c -I/usr/include/postgresql -lpq -lpthread`
- `gcc -o my_program -g  main.c -I/usr/include/postgresql -lpq -lpthread ; gcc -o client Client-Server/client.c`
- `gcc -o my_program -g  main.c -I/usr/include/postgresql -lpq -lpthread ; gcc -o client Client-Server/client.c ; gcc -o PartitaEXE Partita/Partita.c`
- `valgrind --leak-check=full --track-origins=yes ./my_program`
- `make clean && make`

# Per scaricare pg
- `sudo apt-get install --reinstall libpq-dev`

# Schema Database
![Schema Concettuale Database](https://github.com/FlorindoDev/TelefonoSenzaFili/blob/master/Database%20LSO.jpg)

# Documentazione
![Documentazione TelefonoSenzaFili](https://github.com/FlorindoDev/TelefonoSenzaFili/blob/master/Doc%20LSO.pdf)


