DROP VIEW ranking;
DROP TABLE imprezy;
DROP TABLE zawodnicy;
DROP TABLE sponsorzy;
DROP TABLE miejsca;
DROP TABLE final;

CREATE TABLE sponsorzy (
 id_sponsora INTEGER PRIMARY KEY,
 nazwa_sponsora VARCHAR2(50)
);

CREATE TABLE zawodnicy (
 id_zawodnika INTEGER PRIMARY KEY,
 nazwa_zawodnika VARCHAR2(30),
 id_sponsora INTEGER REFERENCES sponsorzy ON DELETE CASCADE NOT NULL,
 ranking INTEGER UNIQUE
);

CREATE TABLE imprezy (
 nazwa_imprezy VARCHAR2(30) PRIMARY KEY
);

CREATE TABLE miejsca (
 id_zawodnika INTEGER REFERENCES zawodnicy ON DELETE CASCADE NOT NULL,
 id_imprezy VARCHAR2(30) REFERENCES imprezy ON DELETE CASCADE NOT NULL,
 punkty INTEGER NOT NULL, 
 miejsce INTEGER NOT NULL,
 PRIMARY KEY(id_zawodnika, id_imprezy)
);

CREATE TABLE final (
 pozycja INTEGER,
 nazwa_zawodnika VARCHAR2(30),
 punkty INTEGER
);

INSERT INTO sponsorzy VALUES (0, 'IBM');
INSERT INTO sponsorzy VALUES (1, 'Prokom');
INSERT INTO sponsorzy VALUES (2, 'FSM Bielsko-Biala');
INSERT INTO sponsorzy VALUES (3, 'Microsoft');
INSERT INTO sponsorzy VALUES (4, 'Ministerstwo Kultury i Dziedzictwa Narodowego');
INSERT INTO sponsorzy VALUES (5, 'StOK');
INSERT INTO sponsorzy VALUES (6, 'Oracle');

INSERT INTO zawodnicy VALUES (0, 'Gerry Conway', 2, 234);
INSERT INTO zawodnicy VALUES (1, 'Ian Anderson', 1, 2);
INSERT INTO zawodnicy VALUES (2, 'Kerry Minnear', 2, 4);
INSERT INTO zawodnicy VALUES (3, 'Jan Akkerman', 3, 34);
INSERT INTO zawodnicy VALUES (4, 'Ray Shulman', 0, 12);
INSERT INTO zawodnicy VALUES (5, 'Peter Hammil', 6, 14);
INSERT INTO zawodnicy VALUES (6, 'Robert Fripp', 1, 101);
INSERT INTO zawodnicy VALUES (7, 'David Sanchez', 4, NULL);
INSERT INTO zawodnicy VALUES (8, 'Charles Mingus', 5, 28);
INSERT INTO zawodnicy VALUES (9, 'Clive Bunker', 5, 8);
INSERT INTO zawodnicy VALUES (10, 'Peter Green', 5, 9);

INSERT INTO imprezy VALUES ('Jazz Jamboree');
INSERT INTO imprezy VALUES ('Warszawska Jesien');
INSERT INTO imprezy VALUES ('Sopot 2000');
INSERT INTO imprezy VALUES ('Bieg dookola ratusza');
INSERT INTO imprezy VALUES ('Maraton nowojorski');
INSERT INTO imprezy VALUES ('Cropredy Festivall');
INSERT INTO imprezy VALUES ('Sydney 2000');
INSERT INTO imprezy VALUES ('TROMBA');
INSERT INTO imprezy VALUES ('Rejestracja na zajecia');
INSERT INTO imprezy VALUES ('Egzamin z BD');


