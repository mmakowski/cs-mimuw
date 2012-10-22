/*
*************************************************************************
* STUDIO NAGRANIOWE                                                     *
* projekt zaliczeniowy z BD LAB                                         *
* autor: Maciek Makowski (mm189188)                                     *
*                                                                       *
* skrypt tworzacy pakiety procedur i funkcji                            *
*************************************************************************/

/*
*************************************************************************
* pakiet klient                                                         *
*************************************************************************/

  CREATE OR REPLACE PROCEDURE loguj(
    klogin klienci.login%TYPE,
    khaslo klienci.haslo%TYPE)
  IS
    val NUMBER;
  BEGIN
    SELECT COUNT(*) INTO val FROM (
      SELECT * FROM klienci WHERE login = klogin AND haslo = khaslo);
    IF VAL = 1 THEN
      INSERT INTO zalogowani_uzytkownicy VALUES (klogin, SYSDATE);
      COMMIT;
    END IF;  
  END;
.
run
SHOW ERRORS PROCEDURE loguj;
------------------------------------------------------------------------
  CREATE OR REPLACE PROCEDURE wyloguj(
    klogin klienci.login%TYPE)
  IS
  BEGIN
    DELETE FROM zalogowani_uzytkownicy WHERE login = klogin;
    COMMIT;
  END;
.
run
SHOW ERRORS PROCEDURE wyloguj;
------------------------------------------------------------------------
  CREATE OR REPLACE FUNCTION zalogowany(
    klogin klienci.login%TYPE)
    RETURN NUMBER
  IS
    val NUMBER;
  BEGIN
    SELECT COUNT(*) INTO val FROM (
      SELECT * FROM zalogowani_uzytkownicy WHERE login = klogin);
    RETURN val;
  END;
.
run
SHOW ERRORS FUNCTION zalogowany;
------------------------------------------------------------------------
  CREATE OR REPLACE FUNCTION dobry_login(
    klogin klienci.login%TYPE)
    RETURN NUMBER
  IS
    val NUMBER;
  BEGIN
    SELECT COUNT(*) INTO val FROM (
      SELECT * FROM klienci WHERE login = klogin);
    IF val > 0 THEN
      RETURN 0; 
    ELSE 
      RETURN 1;
    END IF;
  END;
.
run
SHOW ERRORS FUNCTION dobry_login;
------------------------------------------------------------------------
  CREATE OR REPLACE PROCEDURE rejestruj(
    klogin klienci.login%TYPE, 
    khaslo klienci.haslo%TYPE,
    knazwa klienci.nazwa%TYPE,
    knip klienci.nip%TYPE,
    kadres klienci.adres%TYPE,
    kemail klienci.email%TYPE,
    ktelefon klienci.telefon%TYPE,
    kfax klienci.fax%TYPE)
  IS 
  BEGIN
    INSERT INTO klienci VALUES (klogin, khaslo, knazwa, kadres, knip, ktelefon, kfax, kemail);
    INSERT INTO niezweryfikowani_uzytkownicy VALUES (klogin, SYSDATE);
  END;
.
run
SHOW ERRORS PROCEDURE rejestruj;
------------------------------------------------------------------------
  CREATE OR REPLACE PROCEDURE nowa_sesja(
    klient nagrania.klient%TYPE, 
    nazwa nagrania.nazwa%TYPE,
    poczatek VARCHAR2,
    koniec VARCHAR2,
    realizator nagrania.realizator%TYPE,
    studio nagrania.studio%TYPE)
  IS 
  BEGIN
    INSERT INTO nagrania VALUES (sek_nagrania_pk.NEXTVAL, nazwa , TO_DATE(poczatek, 'DD.MM.YYYY HH24:MI'), TO_DATE(koniec, 'DD.MM.YYYY HH24:MI'), studio, realizator, klient, 0, 'NIE', 0);
    COMMIT;
  END;
.
run
SHOW ERRORS PROCEDURE nowa_sesja;
------------------------------------------------------------------------
  CREATE OR REPLACE PROCEDURE rezygnuj_z_sesji(
    kl nagrania.klient%TYPE, 
    nrnag nagrania.nr%TYPE)
  IS 
  BEGIN
    DELETE FROM nagrania WHERE klient=kl AND nr=nrnag;
    COMMIT;
  END;
.
run
SHOW ERRORS PROCEDURE rezygnuj_z_sesji;
------------------------------------------------------------------------  
  CREATE OR REPLACE PROCEDURE anuluj_sesje(nr_nagr nagrania.nr%TYPE) IS
  BEGIN
    DELETE FROM muzyk_nagranie WHERE nr_nagrania = nr_nagr;
    UPDATE nagrania SET anulowane='TAK' WHERE nr = nr_nagr;
  END;
.
run
SHOW ERRORS PROCEDURE anuluj_sesje;
------------------------------------------------------------------------  
  CREATE OR REPLACE PROCEDURE nowy_muzyk(
    imie muzycy.imie%TYPE,
    nazwisko muzycy.nazwisko%TYPE,
    nr_dokumentu muzycy.nr_dokumentu%TYPE,
    rodzaj_dokumentu muzycy.rodzaj_dokumentu%TYPE)
  IS 
  BEGIN
    INSERT INTO muzycy VALUES (sek_muzycy_pk.NEXTVAL, imie, nazwisko, nr_dokumentu, rodzaj_dokumentu);
    COMMIT;
  END;
.
run
SHOW ERRORS PROCEDURE nowy_muzyk;
------------------------------------------------------------------------
  CREATE OR REPLACE PROCEDURE dodaj_muzyka(
    nr_nagr muzyk_nagranie.nr_nagrania%TYPE,
    nr_muz muzyk_nagranie.nr_muzyka%TYPE)
  IS
    v NUMBER; 
  BEGIN
    SELECT COUNT(*) INTO v FROM muzyk_nagranie WHERE nr_muzyka = nr_muz AND nr_nagrania = nr_nagr;
    IF v = 0 THEN 
      INSERT INTO muzyk_nagranie VALUES (nr_muz, nr_nagr);
      COMMIT;
    END IF;
  END;
.
run
SHOW ERRORS PROCEDURE dodaj_muzyka;
------------------------------------------------------------------------    
  CREATE OR REPLACE PROCEDURE usun_muzyka(
    nr_nagr muzyk_nagranie.nr_nagrania%TYPE,
    nr_muz muzyk_nagranie.nr_muzyka%TYPE)
  IS
  BEGIN
    DELETE FROM muzyk_nagranie WHERE nr_muzyka = nr_muz AND nr_nagrania = nr_nagr;
    COMMIT;
  END;
.
run
SHOW ERRORS PROCEDURE usun_muzyka;
------------------------------------------------------------------------
  CREATE OR REPLACE PROCEDURE dodaj_technika(
    nr_nagr technik_nagranie.nr_nagrania%TYPE,
    nr_tech technik_nagranie.nr_technika%TYPE)
  IS
    v NUMBER; 
  BEGIN
    SELECT COUNT(*) INTO v FROM technik_nagranie WHERE nr_technika = nr_tech AND nr_nagrania = nr_nagr;
    IF v = 0 THEN 
      INSERT INTO technik_nagranie VALUES (nr_tech, nr_nagr);
      COMMIT;
    END IF;
  END;
.
run
SHOW ERRORS PROCEDURE dodaj_technika;
------------------------------------------------------------------------    
  CREATE OR REPLACE PROCEDURE usun_technika(
    nr_nagr technik_nagranie.nr_nagrania%TYPE,
    nr_tech technik_nagranie.nr_technika%TYPE)
  IS
  BEGIN
    DELETE FROM technik_nagranie WHERE nr_technika = nr_tech AND nr_nagrania = nr_nagr;
    COMMIT;
  END;
.
run
SHOW ERRORS PROCEDURE usun_technika;
------------------------------------------------------------------------
  CREATE OR REPLACE PROCEDURE dodaj_urzadzenie(
    nr_nagr urzadzenie_nagranie.nr_nagrania%TYPE,
    nr_urz urzadzenie_nagranie.nr_urzadzenia%TYPE)
  IS
    v NUMBER; 
  BEGIN
    SELECT COUNT(*) INTO v FROM urzadzenie_nagranie WHERE nr_urzadzenia = nr_urz AND nr_nagrania = nr_nagr;
    IF v = 0 THEN 
      INSERT INTO urzadzenie_nagranie VALUES (nr_urz, nr_nagr);
      COMMIT;
    END IF;
  END;
.
run
SHOW ERRORS PROCEDURE dodaj_urzadzenie;
------------------------------------------------------------------------    
  CREATE OR REPLACE PROCEDURE usun_urzadzenie(
    nr_nagr urzadzenie_nagranie.nr_nagrania%TYPE,
    nr_urz urzadzenie_nagranie.nr_urzadzenia%TYPE)
  IS
  BEGIN
    DELETE FROM urzadzenie_nagranie WHERE nr_urzadzenia = nr_urz AND nr_nagrania = nr_nagr;
    COMMIT;
  END;
.
run
SHOW ERRORS PROCEDURE usun_urzadzenie;
------------------------------------------------------------------------    
  CREATE OR REPLACE PROCEDURE usun_realizatora(
    nr_nagr nagrania.nr%TYPE)
  IS
  BEGIN
    UPDATE nagrania SET realizator = NULL WHERE nr = nr_nagr;
    COMMIT;
  END;
.
run
SHOW ERRORS PROCEDURE usun_realizatora;
------------------------------------------------------------------------    
  CREATE OR REPLACE PROCEDURE zatwierdz IS
  BEGIN
    COMMIT;
  END;
.
run
SHOW ERRORS PROCEDURE zatwierdz;
------------------------------------------------------------------------
  CREATE OR REPLACE FUNCTION koszt_personelu(nr_nagr nagrania.nr%TYPE) RETURN NUMBER IS
    val NUMBER;
  BEGIN
    SELECT NVL(SUM(stawka), 0) INTO val FROM (
      (SELECT stawka FROM technicy, technik_nagranie
        WHERE technicy.nr = nr_technika AND nr_nagrania = nr_nagr)
      UNION
      (SELECT stawka FROM realizatorzy, nagrania 
        WHERE realizatorzy.nr = nagrania.realizator AND nagrania.nr = nr_nagr));
    RETURN val;
  END;
.
run
SHOW ERRORS FUNCTION koszt_personelu;
------------------------------------------------------------------------  
  CREATE OR REPLACE FUNCTION koszt_studia(nr_nagr nagrania.nr%TYPE) RETURN NUMBER IS
    val NUMBER;
  BEGIN
    SELECT stawka INTO val FROM studia, nagrania 
      WHERE studia.nazwa = nagrania.studio AND nagrania.nr = nr_nagr;
    RETURN val;
  END;
.
run
SHOW ERRORS FUNCTION koszt_studia;
------------------------------------------------------------------------
  CREATE OR REPLACE FUNCTION koszt_urzadzen(nr_nagr nagrania.nr%TYPE) RETURN NUMBER IS
    val NUMBER;
  BEGIN
    SELECT NVL(SUM(stawka), 0) INTO val FROM (
      SELECT stawka FROM urzadzenia, urzadzenie_nagranie
        WHERE urzadzenia.nr = nr_urzadzenia AND nr_nagrania = nr_nagr);
    RETURN val;
  END;
.
run
SHOW ERRORS FUNCTION koszt_urzadzen;
------------------------------------------------------------------------
  CREATE OR REPLACE FUNCTION technik_wolny(
    nr_tech technik_nagranie.nr_technika%TYPE,
    pocz VARCHAR2,
    kon VARCHAR2) 
    RETURN NUMBER 
  IS
    val NUMBER;
  BEGIN
    SELECT COUNT(*) INTO val FROM (
      SELECT nagrania.nr FROM nagrania, technik_nagranie
        WHERE technik_nagranie.nr_nagrania = nagrania.nr AND
              technik_nagranie.nr_technika = nr_tech AND
              ((nagrania.poczatek >= TO_DATE(pocz, 'DD.MM.YYYY HH24:MI') AND nagrania.poczatek <= TO_DATE(kon, 'DD.MM.YYYY HH24:MI')) OR
               (nagrania.koniec >= TO_DATE(pocz, 'DD.MM.YYYY HH24:MI') AND nagrania.koniec <= TO_DATE(kon, 'DD.MM.YYYY HH24:MI'))));
    IF val = 0 THEN 
      RETURN 1;
    ELSE
      RETURN 0;
    END IF;
  END;
.
run
SHOW ERRORS FUNCTION technik_wolny;
------------------------------------------------------------------------
  CREATE OR REPLACE FUNCTION urzadzenie_wolne(
    nr_urz urzadzenie_nagranie.nr_urzadzenia%TYPE,
    pocz VARCHAR2,
    kon VARCHAR2) 
    RETURN NUMBER 
  IS
    val NUMBER;
  BEGIN
    SELECT COUNT(*) INTO val FROM (
      SELECT nagrania.nr FROM nagrania, urzadzenie_nagranie
        WHERE urzadzenie_nagranie.nr_nagrania = nagrania.nr AND
              urzadzenie_nagranie.nr_urzadzenia = nr_urz AND
              ((nagrania.poczatek >= TO_DATE(pocz, 'DD.MM.YYYY HH24:MI') AND nagrania.poczatek <= TO_DATE(kon, 'DD.MM.YYYY HH24:MI')) OR
               (nagrania.koniec >= TO_DATE(pocz, 'DD.MM.YYYY HH24:MI') AND nagrania.koniec <= TO_DATE(kon, 'DD.MM.YYYY HH24:MI'))));
    IF val = 0 THEN 
      RETURN 1;
    ELSE
      RETURN 0;
    END IF;
  END;
.
run
SHOW ERRORS FUNCTION urzadzenie_wolne;
------------------------------------------------------------------------
  CREATE OR REPLACE FUNCTION studio_wolne(
    stud nagrania.studio%TYPE,
    pocz VARCHAR2,
    kon VARCHAR2) 
    RETURN NUMBER 
  IS
    val NUMBER;
  BEGIN
    SELECT COUNT(*) INTO val FROM (
      SELECT nagrania.nr FROM nagrania
        WHERE nagrania.studio = stud AND
              ((nagrania.poczatek >= TO_DATE(pocz, 'DD.MM.YYYY HH24:MI') AND nagrania.poczatek <= TO_DATE(kon, 'DD.MM.YYYY HH24:MI')) OR
               (nagrania.koniec >= TO_DATE(pocz, 'DD.MM.YYYY HH24:MI') AND nagrania.koniec <= TO_DATE(kon, 'DD.MM.YYYY HH24:MI'))));
    IF val = 0 THEN 
      RETURN 1;
    ELSE
      RETURN 0;
    END IF;
  END;
.
run
SHOW ERRORS FUNCTION studio_wolne;
------------------------------------------------------------------------
  CREATE OR REPLACE FUNCTION realizator_wolny(
    real nagrania.realizator%TYPE,
    pocz VARCHAR2,
    kon VARCHAR2) 
    RETURN NUMBER 
  IS
    val NUMBER;
  BEGIN
    SELECT COUNT(*) INTO val FROM (
      SELECT nagrania.nr FROM nagrania
        WHERE nagrania.realizator = real AND
              ((nagrania.poczatek >= TO_DATE(pocz, 'DD.MM.YYYY HH24:MI') AND nagrania.poczatek <= TO_DATE(kon, 'DD.MM.YYYY HH24:MI')) OR
               (nagrania.koniec >= TO_DATE(pocz, 'DD.MM.YYYY HH24:MI') AND nagrania.koniec <= TO_DATE(kon, 'DD.MM.YYYY HH24:MI'))));
    IF val = 0 THEN 
      RETURN 1;
    ELSE
      RETURN 0;
    END IF;
  END;
.
run
SHOW ERRORS FUNCTION realizator_wolny;

