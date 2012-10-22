/*
*************************************************************************
* STUDIO NAGRANIOWE                                                     *
* projekt zaliczeniowy z BD LAB                                         *
* autor: Maciek Makowski (mm189188)                                     *
*                                                                       *
* skrypt tworzacy tabele sekwencje, perspektywy i wyzwalacze            *
*************************************************************************/

/*
*************************************************************************
* tabele slownikowe                                                     *
*************************************************************************/

/* 
* rodzaje dokumentow */
CREATE TABLE rodzaje_dokumentow (
  nazwa VARCHAR2(30)
    CONSTRAINT RODZAJE_DOKUMENTOW_PK PRIMARY KEY
);

/* 
* rodzaje urzadzen */
CREATE TABLE rodzaje_urzadzen (
  nazwa VARCHAR2(50)
    CONSTRAINT RODZAJE_URZADZEN_PK PRIMARY KEY
);

/* 
* producenci urzadzen */
CREATE TABLE producenci_urzadzen (
  nazwa VARCHAR2(30)
    CONSTRAINT PRODUCENCI_URZADZEN_PK PRIMARY KEY
);

/*
*************************************************************************
* tabele podstawowe                                                     *
*************************************************************************/

/*
* muzycy */
CREATE TABLE muzycy (
  nr NUMBER(4)
    CONSTRAINT MUZYCY_PK PRIMARY KEY,
  imie VARCHAR2(30) NOT NULL,
  nazwisko VARCHAR2(30) NOT NULL,
  nr_dokumentu VARCHAR(30) NOT NULL,
  rodzaj_dokumentu VARCHAR(30)
    CONSTRAINT MUZYCY_FK_RODZAJE_DOKUMENTOW REFERENCES rodzaje_dokumentow
    NOT NULL
);

ALTER TABLE muzycy ADD CONSTRAINT MUZYCY_UQ_IM_NAZ UNIQUE(imie, nazwisko);

CREATE SEQUENCE sek_muzycy_pk;

/*
* klienci */
CREATE TABLE klienci (
  login VARCHAR2(16)
    CONSTRAINT KLIENCI_PK PRIMARY KEY,
  haslo VARCHAR2(16) NOT NULL,
  nazwa VARCHAR2(100) NOT NULL,
  adres VARCHAR2(100) NOT NULL,
  nip VARCHAR2(13) NOT NULL,
  telefon VARCHAR2(16),
  fax VARCHAR2(16),
  email VARCHAR2(50)
);

CREATE SEQUENCE sek_klienci_pk;

CREATE VIEW pers_klienci AS SELECT login, nazwa, adres, nip, telefon, fax, email FROM klienci;

/*
* studia */
CREATE TABLE studia (
  nazwa VARCHAR2(30)
    CONSTRAINT STUDIA_PK PRIMARY KEY,
  stawka NUMBER(6,2)
    DEFAULT 50
    CONSTRAINT STUDIA_STAWKAgte0 CHECK (stawka >= 0) 
    NOT NULL
);

/*
* technicy */
CREATE TABLE technicy (
  nr NUMBER(3)
    CONSTRAINT TECHNICY_PK PRIMARY KEY,
  imie VARCHAR2(30) NOT NULL,
  nazwisko VARCHAR(30) NOT NULL,
  stawka NUMBER(5,2)
    DEFAULT 10
    CONSTRAINT TECHNICY_STAWKAgte0 CHECK (stawka >= 0) 
    NOT NULL
);

CREATE SEQUENCE sek_technicy_pk;

/*
* realizatorzy */
CREATE TABLE realizatorzy (
  nr NUMBER(3)
    CONSTRAINT REALIZATORZY_PK PRIMARY KEY,
  imie VARCHAR2(30) NOT NULL,
  nazwisko VARCHAR(30) NOT NULL,
  stawka NUMBER(5,2)
    DEFAULT 30
    CONSTRAINT REALIZATORZY_STAWKAgte0 CHECK (stawka >= 0) 
    NOT NULL
);

CREATE SEQUENCE sek_realizatorzy_pk;

CREATE TRIGGER realizatorzy_insert BEFORE INSERT ON realizatorzy
FOR EACH ROW
BEGIN
  SELECT sek_realizatorzy_pk.NEXTVAL INTO :new.nr FROM dual;
END;
.
run
SHOW ERRORS TRIGGER realizatorzy_insert;
/*
* urzadzenia */
CREATE TABLE urzadzenia (
  nr NUMBER(6)
    CONSTRAINT URZADZENIA_PK PRIMARY KEY,
  rodzaj VARCHAR2(50)
    CONSTRAINT URZADZENIA_FK_RODZAJE_URZADZEN REFERENCES rodzaje_urzadzen
    NOT NULL,
  producent VARCHAR2(30)
    CONSTRAINT URZADZENIA_FK_PRODUC_URZADZEN REFERENCES producenci_urzadzen
    NOT NULL,
  model VARCHAR2(50) NOT NULL,
  nr_seryjny VARCHAR2(30) NOT NULL,
  stawka NUMBER(6,2)
    DEFAULT 5
    CONSTRAINT URZADZENIA_STAWKAgte0 CHECK (stawka >= 0) 
    NOT NULL
);

CREATE SEQUENCE sek_urzadzenia_pk;

/*
* nagrania */
CREATE TABLE nagrania (
  nr NUMBER(7)
    CONSTRAINT NAGRANIA_PK PRIMARY KEY,
  nazwa VARCHAR2(50),
  poczatek DATE NOT NULL,
  koniec DATE NOT NULL,
  studio VARCHAR2(30)
    CONSTRAINT NAGRANIA_FK_STUDIA REFERENCES studia
    ON DELETE CASCADE
    NOT NULL,
  realizator NUMBER(3)
    CONSTRAINT NAGRANIA_FK_REALIZATORZY REFERENCES realizatorzy,
  klient VARCHAR2(16)
    CONSTRAINT NAGRANIA_FK_KLIENCI REFERENCES klienci
    ON DELETE CASCADE
    NOT NULL,
  rabat NUMBER(7,2)
    DEFAULT 0
    CONSTRAINT NAGRANIA_RABATgte0 CHECK (rabat >= 0) 
    NOT NULL,
  anulowane CHAR(3)
    DEFAULT 'NIE'
    NOT NULL,
  zaplacone NUMBER(8,2)
    DEFAULT 0
    NOT NULL
);

ALTER TABLE nagrania ADD CONSTRAINT NAGRANIA_KONIECgtPOCZ CHECK (koniec > poczatek);
ALTER TABLE nagrania ADD CONSTRAINT NAGRANIA_UQ_POCZ_KON_STUD UNIQUE (poczatek, koniec, studio);

CREATE SEQUENCE sek_nagrania_pk;

/*
*************************************************************************
* tabele realizujace zwiazki wiele-do-wielu                             *
*************************************************************************/

/*
* urzadzenia wykorzystywany przy nagraniu */
CREATE TABLE urzadzenie_nagranie (
  nr_urzadzenia NUMBER(6)
    CONSTRAINT URZADZENIE_NAGRANIE_FK_URZADZ REFERENCES urzadzenia
    ON DELETE CASCADE
    NOT NULL,
  nr_nagrania NUMBER(7)
    CONSTRAINT URZADZENIE_NAGRANIE_FK_NAGR REFERENCES nagrania
    ON DELETE CASCADE
    NOT NULL
);

ALTER TABLE urzadzenie_nagranie ADD CONSTRAINT URZADZENIE_NAGRANIE_UQ_URZ_NAG UNIQUE(nr_urzadzenia, nr_nagrania);

/*
* technicy pracujacy przy nagraniu */
CREATE TABLE technik_nagranie (
  nr_technika NUMBER(3)
    CONSTRAINT TECHNIK_NAGRANIE_FK_TECHNICY REFERENCES technicy
    ON DELETE CASCADE
    NOT NULL,
  nr_nagrania NUMBER(7)
    CONSTRAINT TECHNIK_NAGRANIE_FK_NAGRANIA REFERENCES nagrania
    ON DELETE CASCADE
    NOT NULL
);

ALTER TABLE technik_nagranie ADD CONSTRAINT TECHNIK_NAGRANIE_UQ_URZ_NAG UNIQUE(nr_technika, nr_nagrania);

/*
* muzycy bioracy udzial w nagraniu */
CREATE TABLE muzyk_nagranie (
  nr_muzyka NUMBER(4)
    CONSTRAINT MUZYK_NAGRANIE_FK_MUZYCY REFERENCES muzycy
    ON DELETE CASCADE
    NOT NULL,
  nr_nagrania NUMBER(7)
    CONSTRAINT MUZYK_NAGRANIE_FK_NAGRANIA REFERENCES nagrania
    ON DELETE CASCADE
    NOT NULL
);

ALTER TABLE muzyk_nagranie ADD CONSTRAINT MUZYK_NAGRANIE_UQ_URZ_NAG UNIQUE(nr_muzyka, nr_nagrania);

/*
*************************************************************************
* tabele systemowe                                                      *
*************************************************************************/

/* 
* zalogowani uzytkownicy */
CREATE TABLE zalogowani_uzytkownicy (
  login VARCHAR2(16)
    CONSTRAINT ZALOG_UZYTK_PK PRIMARY KEY
    CONSTRAINT ZALOG_UZYKT_FK_KLIENCI REFERENCES klienci,
  czas DATE NOT NULL
);

/* 
* uzytkownicy do zweryfikowania*/
CREATE TABLE niezweryfikowani_uzytkownicy (
  login VARCHAR2(16)
    CONSTRAINT NIEZWER_UZYTK_PK PRIMARY KEY
    CONSTRAINT NIEZWER_UZYKT_FK_KLIENCI REFERENCES klienci,
  czas DATE NOT NULL
);

/*
* perspektywa z uzytkownikami dla Oracle*Forms*/
CREATE VIEW forms_uzytkownicy AS 
  SELECT klienci.login, haslo, adres, nip, telefon, fax, email, czas 
    FROM klienci, niezweryfikowani_uzytkownicy 
    WHERE klienci.login = niezweryfikowani_uzytkownicy.login;

CREATE TRIGGER forms_uzytkownicy_update INSTEAD OF UPDATE ON forms_uzytkownicy
BEGIN
  UPDATE klienci SET haslo = :new.haslo WHERE login = :old.login;
END;
.
run
show errors trigger forms_uzytkownicy_update;

CREATE TRIGGER forms_uzytkownicy_delete INSTEAD OF DELETE ON forms_uzytkownicy
BEGIN
  DELETE FROM niezweryfikowani_uzytkownicy WHERE login = :old.login;
  DELETE FROM klienci WHERE login = :old.login;
END;
.
run
show errors trigger forms_uzytkownicy_delete;