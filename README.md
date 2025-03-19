# Documentazione
<a target="_blank" href="https://github.com/FlorindoDev/TelefonoSenzaFili/blob/master/Doc%20LSO.pdf">Documentazione TelefonoSenzaFili</a>

# SetUp

## Primo passo
Eseguire lo script **ImageBuilder**<br>
- `ImageBuilder.sh` se si è su un sistema Unix-like<br>
- `ImageBuilder.bat` se si è su Windows<br>

## Secondo passo
- Eseguire il file `docker-compose` con il seguente comando:
  ```sh
  docker compose up -d
  ```
- Ovviamente il comando va eseguito nella directory in cui risiede il file `docker-compose`.<br>

## Terzo passo
- Entrare nella console di un container client con il seguente comando:
  ```sh
  docker exec -it <container_id> /bin/bash
  ```
  - `-i`: mantiene l'input interattivo.<br>
  - `-t`: assegna un terminale pseudo-TTY.<br>

## Quarto passo
Avviare il client nel conteiner con
```
./client
```

## Quinto passo
- Giocare al gioco 🎮<br>

# Fonti
- **libpq eseguire query**: [https://www.postgresql.org/docs/9.4/libpq-exec.html](https://www.postgresql.org/docs/9.4/libpq-exec.html)
- **libpq Creare connessione**: [https://www.postgresql.org/docs/9.4/libpq-connect.html#LIBPQ-CONNSTRING](https://www.postgresql.org/docs/9.4/libpq-connect.html#LIBPQ-CONNSTRING)
- **Traduzione**: [https://libretranslate.com/](https://libretranslate.com/)
