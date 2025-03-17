CREATE TABLE utente (
    nome VARCHAR(20) PRIMARY KEY,
    password_utente VARCHAR(20) NOT NULL,
    lingua VARCHAR(20) NOT NULL
);

ALTER TABLE utente
ADD CONSTRAINT check_lingua
CHECK (lingua IN ('it', 'en', 'es', 'fr', 'de'));