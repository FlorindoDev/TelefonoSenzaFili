# Documentazione
<a target="_blanck" href="https://github.com/FlorindoDev/TelefonoSenzaFili/blob/master/Doc%20LSO.pdf">Documentazione TelefonoSenzaFili</a>

# SetUp

## Primo passo
Eseguire lo script `ImageBuilder`<br>
- `ImageBuilder.sh` se si è su un sistma `Unix-like` <br>
- `ImageBuilder.bat` se su è su `windows` <br>

## Secondo passo
- Eseguire il file `docker-compose` con il seguente comando `docker compose up -d` <br>
- Ovviamente il comando va fatto mentre si è nella direcotry in cui risede il `docker-compose` <br>

## Terzo passo
- Entrare nella console di un continer clinet , con il seguente comando `docker exec -it <container_id> /bin/bash` <br>
- i: mantiene l'input interattivo. <br>
- t: assegna un terminale pseudo-TTY. <br>

## Quarto passo
- Giocare al gioco <br>

# Fonti
- libpq eseguire quary: https://www.postgresql.org/docs/9.4/libpq-exec.html
- libpq Creare connessione: https://www.postgresql.org/docs/9.4/libpq-exec.html](https://www.postgresql.org/docs/9.4/libpq-connect.html#LIBPQ-CONNSTRING
- traduzione: https://libretranslate.com/




