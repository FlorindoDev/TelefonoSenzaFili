# Usa Ubuntu come base
FROM ubuntu:20.04

# Evita le richieste interattive durante l'installazione
ENV DEBIAN_FRONTEND=noninteractive

# Aggiorna il sistema e installa i pacchetti necessari
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    make \
    postgresql-client \
    libpq-dev \
    git \
    micro \
    sudo \
    net-tools \
    && rm -rf /var/lib/apt/lists/*
    # gcc -o my_program main.c -I/usr/include/postgresql -lpq \
    # gcc -o client ProveSocket/client.c

# Imposta una directory di lavoro
WORKDIR /usr/src/app

# Copia i file del progetto nella directory di lavoro (opzionale)
COPY . .

# Comando di default (opzionale, può essere sovrascritto)
CMD ["bash"]