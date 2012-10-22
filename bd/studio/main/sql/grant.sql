/*
*************************************************************************
* STUDIO NAGRANIOWE                                                     *
* projekt zaliczeniowy z BD LAB                                         *
* autor: Maciek Makowski (mm189188)                                     *
*                                                                       *
* skrypt nadajacy uprawnienia do korzystania z tabel                    *
*************************************************************************/

GRANT SELECT ON rodzaje_dokumentow TO scott;
GRANT SELECT ON rodzaje_urzadzen TO scott;
GRANT SELECT ON producenci_urzadzen TO scott;

GRANT SELECT ON pers_klienci TO scott;
GRANT SELECT ON studia TO scott;
GRANT SELECT ON urzadzenia TO scott;
GRANT SELECT ON muzycy TO scott;
GRANT SELECT ON technicy TO scott;
GRANT SELECT ON realizatorzy TO scott;
GRANT SELECT ON nagrania TO scott;
GRANT SELECT ON technik_nagranie TO scott;
GRANT SELECT ON urzadzenie_nagranie TO scott;
GRANT SELECT ON muzyk_nagranie TO scott;

GRANT EXECUTE ON loguj TO scott;
GRANT EXECUTE ON wyloguj TO scott;
GRANT EXECUTE ON zalogowany TO scott;
GRANT EXECUTE ON dobry_login TO scott;
GRANT EXECUTE ON rejestruj TO scott;
GRANT EXECUTE ON nowy_muzyk TO scott;
GRANT EXECUTE ON nowa_sesja TO scott;
GRANT EXECUTE ON rezygnuj_z_sesji TO scott;
GRANT EXECUTE ON anuluj_sesje TO scott;
GRANT EXECUTE ON nowy_muzyk TO scott;
GRANT EXECUTE ON dodaj_muzyka TO scott;
GRANT EXECUTE ON usun_muzyka TO scott;
GRANT EXECUTE ON dodaj_technika TO scott;
GRANT EXECUTE ON usun_technika TO scott;
GRANT EXECUTE ON dodaj_urzadzenie TO scott;
GRANT EXECUTE ON usun_urzadzenie TO scott;
GRANT EXECUTE ON usun_realizatora TO scott;
GRANT EXECUTE ON dodaj_urzadzenie TO scott;
GRANT EXECUTE ON koszt_personelu TO scott;
GRANT EXECUTE ON koszt_studia TO scott;
GRANT EXECUTE ON koszt_urzadzen TO scott;
GRANT EXECUTE ON technik_wolny TO scott;
GRANT EXECUTE ON realizator_wolny TO scott;
GRANT EXECUTE ON studio_wolne TO scott;
GRANT EXECUTE ON urzadzenie_wolne TO scott;

