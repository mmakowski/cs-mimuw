/*
*************************************************************************
* STUDIO NAGRANIOWE                                                     *
* projekt zaliczeniowy z BD LAB                                         *
* autor: Maciek Makowski (mm189188)                                     *
*                                                                       *
* skrypt czyszczacy baze z wczesniejszych obiektow                      *
*************************************************************************/

DROP PROCEDURE loguj;
DROP PROCEDURE wyloguj;
DROP FUNCTION zalogowany;
DROP FUNCTION dobry_login;
DROP PROCEDURE rejestruj;
DROP PROCEDURE nowy_muzyk;
DROP PROCEDURE nowa_sesja;
DROP PROCEDURE rezygnuj_z_sesji;
DROP PROCEDURE anuluj_sesje;
DROP PROCEDURE dodaj_muzyka;
DROP PROCEDURE usun_muzyka;
DROP PROCEDURE dodaj_technika;
DROP PROCEDURE usun_technika;
DROP PROCEDURE dodaj_urzadzenie;
DROP PROCEDURE usun_urzadzenie;
DROP PROCEDURE usun_realizatora;
DROP FUNCTION koszt_personelu;
DROP FUNCTION koszt_studia;
DROP FUNCTION koszt_urzadzen;
DROP FUNCTION technik_wolny;
DROP FUNCTION realizator_wolny;
DROP FUNCTION studio_wolne;
DROP FUNCTION urzadzenie_wolne;

DROP TRIGGER realizatorzy_insert;
DROP TRIGGER forms_uzytkownicy_delete;
DROP TRIGGER forms_uzytkownicy_update;
DROP VIEW forms_uzytkownicy;
DROP VIEW pers_klienci;
DROP TABLE niezweryfikowani_uzytkownicy;
DROP TABLE zalogowani_uzytkownicy;
DROP TABLE urzadzenie_nagranie;
DROP TABLE technik_nagranie;
DROP TABLE muzyk_nagranie;
DROP TABLE nagrania;
DROP TABLE muzycy;
DROP TABLE klienci;
DROP TABLE studia;
DROP TABLE technicy;
DROP TABLE realizatorzy;
DROP TABLE urzadzenia;
DROP TABLE rodzaje_dokumentow;
DROP TABLE rodzaje_urzadzen;
DROP TABLE producenci_urzadzen;
DROP SEQUENCE sek_nagrania_pk;
DROP SEQUENCE sek_muzycy_pk;
DROP SEQUENCE sek_klienci_pk;
DROP SEQUENCE sek_technicy_pk;
DROP SEQUENCE sek_realizatorzy_pk;
DROP SEQUENCE sek_urzadzenia_pk;

