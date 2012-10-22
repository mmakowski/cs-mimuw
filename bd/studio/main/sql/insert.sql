/*
*************************************************************************
* STUDIO NAGRANIOWE                                                     *
* projekt zaliczeniowy z BD LAB                                         *
* autor: Maciek Makowski (mm189188)                                     *
*                                                                       *
* skrypt wypelniajac tabele przykladowymi danymi                        *
*************************************************************************/

/*
*************************************************************************
* tabele slownikowe                                                     *
*************************************************************************/

/*
* rodzaje dokumentow */
INSERT INTO rodzaje_dokumentow VALUES ('dowód osobisty');
INSERT INTO rodzaje_dokumentow VALUES ('paszport');
INSERT INTO rodzaje_dokumentow VALUES ('prawo jazdy');
INSERT INTO rodzaje_dokumentow VALUES ('legitymacja szkolna/studencka');

/*
* rodzaje urzadzen */
INSERT INTO rodzaje_urzadzen VALUES ('mikser');
INSERT INTO rodzaje_urzadzen VALUES ('mikrofon');
INSERT INTO rodzaje_urzadzen VALUES ('wzmacniacz mocy');
INSERT INTO rodzaje_urzadzen VALUES ('przedwzmacniacz mikrofonowy');
INSERT INTO rodzaje_urzadzen VALUES ('magnetofon cyfrowy DAT');
INSERT INTO rodzaje_urzadzen VALUES ('magnetofon analogowy stereo');
INSERT INTO rodzaje_urzadzen VALUES ('magnetofon analogowy wielo¶ladowy');
INSERT INTO rodzaje_urzadzen VALUES ('cyfrowy procesor efektów');
INSERT INTO rodzaje_urzadzen VALUES ('kolumna g³o¶nikowa');

/*
* producenci urzadzen */
INSERT INTO producenci_urzadzen VALUES ('Mackie');
INSERT INTO producenci_urzadzen VALUES ('Studer');
INSERT INTO producenci_urzadzen VALUES ('AKG');
INSERT INTO producenci_urzadzen VALUES ('Sony');
INSERT INTO producenci_urzadzen VALUES ('Sennheiser');
INSERT INTO producenci_urzadzen VALUES ('Shure');
INSERT INTO producenci_urzadzen VALUES ('T.C. Electronics');
INSERT INTO producenci_urzadzen VALUES ('Lexicon');
INSERT INTO producenci_urzadzen VALUES ('Soundcraft');
INSERT INTO producenci_urzadzen VALUES ('JBL');

/*
* muzycy */
INSERT INTO muzycy VALUES (sek_muzycy_pk.NEXTVAL, 'Ian', 'Anderson', 'AG 12344432', 'paszport');
INSERT INTO muzycy VALUES (sek_muzycy_pk.NEXTVAL, 'Kerry', 'Minnear', 'PDC23464A2', 'prawo jazdy');
INSERT INTO muzycy VALUES (sek_muzycy_pk.NEXTVAL, 'Jan', 'Akkerman', '23-44432', 'paszport');
INSERT INTO muzycy VALUES (sek_muzycy_pk.NEXTVAL, 'David', 'Sanchez', '344432', 'legitymacja szkolna/studencka');

/*
* klienci */
INSERT INTO klienci VALUES ('scott', 'tiger', 'Scott Tiger Records', 'Miklaszewskiego 1/44 02-776 Warszawa', '192-168-12-13', '0608534608', '0226431731', 'm.makowski@students.mimuw.edu.pl');

/*
* studia */
INSERT INTO studia VALUES ('M1', 100);
INSERT INTO studia VALUES ('S1', 800);
INSERT INTO studia VALUES ('REF 3', 40);

/*
* technicy */
INSERT INTO technicy VALUES (sek_technicy_pk.NEXTVAL, 'Franciszek', 'Kufaja', 8);
INSERT INTO technicy VALUES (sek_technicy_pk.NEXTVAL, 'Aleksander', 'Sebiagin', 10);
INSERT INTO technicy VALUES (sek_technicy_pk.NEXTVAL, 'Igor', 'Ko³ywanow', 18);

/*
* realizatorzy */
INSERT INTO realizatorzy(imie, nazwisko, stawka)  VALUES ('Jaros³aw', 'Regulski', 40);
INSERT INTO realizatorzy(imie, nazwisko, stawka)  VALUES ('Lech', 'Dudzik', 35);
INSERT INTO realizatorzy(imie, nazwisko, stawka)  VALUES ('Barbara', 'Okoñ-Makowska', 30);

/*
* urzadzenia */
INSERT INTO urzadzenia VALUES (sek_urzadzenia_pk.NEXTVAL, 'mikser', 'Mackie', '1202-VLZ', '32429 9232-2344', 8.20);
INSERT INTO urzadzenia VALUES (sek_urzadzenia_pk.NEXTVAL, 'magnetofon analogowy stereo', 'Studer', '12R', '232-2344', 15);
INSERT INTO urzadzenia VALUES (sek_urzadzenia_pk.NEXTVAL, 'mikrofon', 'AKG', 'A24', '123244', 5);
INSERT INTO urzadzenia VALUES (sek_urzadzenia_pk.NEXTVAL, 'mikrofon', 'AKG', 'A24', '123245', 5);
INSERT INTO urzadzenia VALUES (sek_urzadzenia_pk.NEXTVAL, 'mikrofon', 'AKG', 'A24', '123246', 5);

/*
* nagrania */
INSERT INTO nagrania VALUES (sek_nagrania_pk.NEXTVAL, '"Sanchez solo" take 0', TO_DATE('05.12.2000 12:30', 'DD.MM.YYYY HH24:MI'), TO_DATE('05.12.2000 21:30', 'DD.MM.YYYY HH24:MI'), 'S1', 3, 'scott', 0, 'NIE', 0);
INSERT INTO nagrania VALUES (sek_nagrania_pk.NEXTVAL, '"Sanchez solo" take 1', TO_DATE('05.12.2001 12:30', 'DD.MM.YYYY HH24:MI'), TO_DATE('05.12.2001 21:30', 'DD.MM.YYYY HH24:MI'), 'S1', 3, 'scott', 0, 'NIE', 0);
INSERT INTO nagrania VALUES (sek_nagrania_pk.NEXTVAL, 'Sesja z IA', TO_DATE('05.02.2001 12:30', 'DD.MM.YYYY HH24:MI'), TO_DATE('05.02.2001 21:30', 'DD.MM.YYYY HH24:MI'), 'M1', 1, 'scott', 0, 'NIE', 50);
INSERT INTO nagrania VALUES (sek_nagrania_pk.NEXTVAL, 'Dziwny zespó³', TO_DATE('05.03.2001 12:30', 'DD.MM.YYYY HH24:MI'), TO_DATE('06.03.2001 01:30', 'DD.MM.YYYY HH24:MI'), 'REF 3', 2, 'scott', 300, 'NIE', 1000);

/*
* muzyk_nagranie */
INSERT INTO technik_nagranie VALUES (1, 3);
INSERT INTO technik_nagranie VALUES (2, 3);
INSERT INTO technik_nagranie VALUES (3, 4);

/*
* muzyk_nagranie */
INSERT INTO muzyk_nagranie VALUES (4, 1);
INSERT INTO muzyk_nagranie VALUES (4, 2);
INSERT INTO muzyk_nagranie VALUES (1, 3);
INSERT INTO muzyk_nagranie VALUES (1, 4);
INSERT INTO muzyk_nagranie VALUES (2, 4);
INSERT INTO muzyk_nagranie VALUES (3, 4);
INSERT INTO muzyk_nagranie VALUES (4, 4);

/*
* urzadzenie_nagranie */
INSERT INTO urzadzenie_nagranie VALUES (3, 3);
INSERT INTO urzadzenie_nagranie VALUES (4, 3);
INSERT INTO urzadzenie_nagranie VALUES (5, 3);
INSERT INTO urzadzenie_nagranie VALUES (2, 3);
INSERT INTO urzadzenie_nagranie VALUES (3, 4);
INSERT INTO urzadzenie_nagranie VALUES (4, 4);
INSERT INTO urzadzenie_nagranie VALUES (1, 4);
INSERT INTO urzadzenie_nagranie VALUES (2, 4);

