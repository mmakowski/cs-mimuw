<?
/*************************************************************************\
* STUDIO NAGRANIOWE                                                       *
* projekt zaliczeniowy z BD LAB                                           *
* autor: Maciek Makowski (mm189188)                                       *
*                                                                         *
* plik z domyslnymi parametrami bazy danych oraz funkcjami bazy           *
\*************************************************************************/

//standardowe definicje

  define("_HOST_","127.0.0.1");
  define("_USER_","scott");
  define("_DB_","");
	define("_PASSWORD_","tiger");

 
///////////////////////////////////////////////////
// Logowanie

function login($login, $pass)
{
  global $db;
  $query = ociparse($db, "begin mm189188.loguj('$login', '$pass'); end;");
  ociexecute($query);
}

///////////////////////////////////////////////////
// Wylogowanie

function logout($login)
{
  global $db;
  $query = ociparse($db, "begin mm189188.wyloguj('$login'); end;");
  ociexecute($query);
}

///////////////////////////////////////////////////
// Sprawdzanie, czy klient jest zalogowany

function logged($login)
{
  global $db;
  
  $query = ociparse($db, "SELECT mm189188.zalogowany('$login') FROM dual");
  ociexecute($query);
  ocifetchinto($query, $arr);
  ocifreestatement($query);
  return (int)$arr[0];
}

///////////////////////////////////////////////////
// Sprawdzanie, czy podany login moze zostac nadany nowemu klientowi

function validLogin($login)
{
  global $db;
  
  $query = ociparse($db, "SELECT mm189188.dobry_login('$login') FROM dual");
  ociexecute($query);
  ocifetchinto($query, $arr);
  ocifreestatement($query);
  return (int)$arr[0];
}

///////////////////////////////////////////////////
// Dodawanie nowego klienta

function addUser($kl_login, $kl_nazwa, $kl_nip, $kl_adres, $kl_email="", $kl_telefon="", $kl_fax="")
{
  global $db;
  mt_srand((double)microtime()*1000000);
  $kl_haslo = mt_rand(1000000, 9999999);
  $query = ociparse($db, "begin mm189188.rejestruj('$kl_login', '$kl_haslo', '$kl_nazwa', '$kl_nip', '$kl_adres', '$kl_email', '$kl_telefon', '$kl_fax'); end;");
  ociexecute($query);
  return !validLogin($kl_login);
}

///////////////////////////////////////////////////
// Nazwa klienta

function userName($userid)
{
  global $db;
  
  $query = ociparse($db, "SELECT nazwa FROM mm189188.pers_klienci WHERE login='$userid'");
  ociexecute($query);
  ocifetchinto($query, $arr);
  return $arr[0];
}

///////////////////////////////////////////////////
// Liczba sesji klienta

function userSessionsCount($userid, $type = 0)
{
  global $db;
  
  //todo: w oraclu jeszcze sprawdzenie daty
  $qs = "SELECT COUNT(*) FROM mm189188.nagrania WHERE klient='$userid'";
  if ($type == 0) $qs .= " AND anulowane='NIE'";
  elseif ($type == 1) $qs .= " AND anulowane='TAK'";
  
  $query = ociparse($db, $qs);
  ociexecute($query);
  ocifetchinto($query, $arr);
  ocifreestatement($query);
  return (int)$arr[0];
}

///////////////////////////////////////////////////
// Dodawanie nowej sesji

function registerSession($klient, $name, $studio, $eng, $from, $to)
{
  global $db;
  if (!$eng) $eng = "NULL";
  $query = ociparse($db, "begin mm189188.nowa_sesja('$klient', '$name', '$from', '$to', $eng, '$studio'); end;");
  ociexecute($query);
  $query = ociparse($db, "SELECT nr from mm189188.nagrania WHERE poczatek=TO_DATE('$from', 'DD.MM.YYYY HH24:MI') AND koniec=TO_DATE('$to', 'DD.MM.YYYY HH24:MI') AND studio='$studio'");
  ociexecute($query);
  if (ocifetchinto($query, $arr)) return (int)$arr[0]; else return 0;
}

///////////////////////////////////////////////////
// Rezygnowanie z sesji

function rollbackSession($klient, $session)
{
  global $db;
  $query = ociparse($db, "begin mm189188.rezygnuj_z_sesji('$klient', $session); end;");
  ociexecute($query);
}

///////////////////////////////////////////////////
// Anulowanie sesji

function cancelSession($session)
{
  global $db;
  $query = ociparse($db, "begin mm189188.anuluj_sesje($session); end;");
  ociexecute($query);
}

///////////////////////////////////////////////////
// Dodawanie nowego muzyka

function newMusician($muz_imie, $muz_nazwisko, $muz_nrdok, $muz_rodzdok)
{
  global $db;
  $query = ociparse($db, "begin mm189188.nowy_muzyk('$muz_imie', '$muz_nazwisko', '$muz_nrdok', '$muz_rodzdok'); end;");
  ociexecute($query);
  $query = ociparse($db, "SELECT nr from mm189188.muzycy WHERE imie='$muz_imie' AND nazwisko='$muz_nazwisko' AND nr_dokumentu='$muz_nrdok' AND rodzaj_dokumentu='$muz_rodzdok'");
  ociexecute($query);
  if (ocifetchinto($query, $arr)) return (int)$arr[0]; else return -1;
}

///////////////////////////////////////////////////
// Dodanie muzyka do sesji

function addMusician($sessionid, $musicianid)
{
  global $db;
  $query = ociparse($db, "begin mm189188.dodaj_muzyka($sessionid, $musicianid); end;");
  ociexecute($query);
}
   
///////////////////////////////////////////////////
// Usuniecie muzyka z sesji

function removeMusician($sessionid, $musicianid)
{
  global $db;
  $query = ociparse($db, "begin mm189188.usun_muzyka($sessionid, $musicianid); end;");
  ociexecute($query);
}

///////////////////////////////////////////////////
// Dodanie technika do sesji

function addTechnician($sessionid, $techid)
{
  global $db;
  $query = ociparse($db, "begin mm189188.dodaj_technika($sessionid, $techid); end;");
  ociexecute($query);
}
   
///////////////////////////////////////////////////
// Usuniecie technika z sesji

function removeTechnician($sessionid, $techid)
{
  global $db;
  $query = ociparse($db, "begin mm189188.usun_technika($sessionid, $techid); end;");
  ociexecute($query);
}

///////////////////////////////////////////////////
// Dodanie urzadzenia do sesji

function addDevice($sessionid, $devid)
{
  global $db;
  $query = ociparse($db, "begin mm189188.dodaj_urzadzenie($sessionid, $devid); end;");
  ociexecute($query);
}
   
///////////////////////////////////////////////////
// Usuniecie urzadzenia z sesji

function removeDevice($sessionid, $devid)
{
  global $db;
  $query = ociparse($db, "begin mm189188.usun_urzadzenie($sessionid, $devid); end;");
  ociexecute($query);
}

///////////////////////////////////////////////////
// Usuniecie realizatora z sesji

function removeEngineer($sessionid)
{
  global $db;
  $query = ociparse($db, "begin mm189188.usun_realizatora($sessionid); end;");
  ociexecute($query);
}
   
///////////////////////////////////////////////////
// Sprawdzenie czy studio jest wolne

function studioAvailable($studioid, $from, $to)
{
  global $db;
  
  $query = ociparse($db, "SELECT mm189188.studio_wolne('$studioid', '$from', '$to') FROM dual");
  ociexecute($query);
  ocifetchinto($query, $arr);
  ocifreestatement($query);
  return (int)$arr[0];
}
   
///////////////////////////////////////////////////
// Sprawdzenie czy realizator jest wolny

function engineerAvailable($engid, $from, $to)
{
  global $db;
  
  $query = ociparse($db, "SELECT mm189188.realizator_wolny($engid, '$from', '$to') FROM dual");
  ociexecute($query);
  ocifetchinto($query, $arr);
  ocifreestatement($query);
  return (int)$arr[0];
}
   
///////////////////////////////////////////////////
// Sprawdzenie czy technik jest wolny

function technicianAvailable($techid, $from, $to)
{
  global $db;
  $query = ociparse($db, "SELECT mm189188.technik_wolny($techid, '$from', '$to') FROM dual");
  ociexecute($query);
  ocifetchinto($query, $arr);
  ocifreestatement($query);
  return (int)$arr[0];
}

///////////////////////////////////////////////////
// Sprawdzenie czy urzadzenie jest wolne

function deviceAvailable($devid, $from, $to)
{
  global $db;
  
  $query = ociparse($db, "SELECT mm189188.urzadzenie_wolne($devid, '$from', '$to') FROM dual");
  ociexecute($query);
  ocifetchinto($query, $arr);
  ocifreestatement($query);
  return (int)$arr[0];
}

///////////////////////////////////////////////////
// Cena personelu

function personnelCostPerHour($sessionid)
{
  global $db;
  
  $query = ociparse($db, "SELECT mm189188.koszt_personelu($sessionid) FROM dual");
  ociexecute($query);
  ocifetchinto($query, $arr);
  ocifreestatement($query);
  return (double)$arr[0];
}

///////////////////////////////////////////////////
// Cena urzadzen

function devicesCostPerHour($sessionid)
{
  global $db;
  
  $query = ociparse($db, "SELECT mm189188.koszt_urzadzen($sessionid) FROM dual");
  ociexecute($query);
  ocifetchinto($query, $arr);
  ocifreestatement($query);
  return (double)$arr[0];
}

///////////////////////////////////////////////////
// Cena studia

function studioCostPerHour($sessionid)
{
  global $db;
  
  $query = ociparse($db, "SELECT mm189188.koszt_studia($sessionid) FROM dual");
  ociexecute($query);
  ocifetchinto($query, $arr);
  ocifreestatement($query);
  return (double)$arr[0];
}

///////////////////////////////////////////////////
// Cena sesji

function sessionCost($sessionid)
{
  global $db;

  $query = ociparse($db, "SELECT TO_CHAR(poczatek, 'YYYYMMDDHH24MI'), TO_CHAR(koniec, 'YYYYMMDDHH24MI'), rabat FROM mm189188.nagrania WHERE nr=$sessionid");
  ociexecute($query);
  ocifetchinto($query, $arr);
  return (double)((personnelCostPerHour($sessionid) +
                   devicesCostPerHour($sessionid) +
                   studioCostPerHour($sessionid)) *
                  ceil((unixTimestamp($arr[1]) - 
                   unixTimestamp($arr[0]))/
                  3600) - 
                  (double)$arr[2]);
}

///////////////////////////////////////////////////
// Poczatek sesji

function sessionStart($sessionid)
{
  global $db;
  $query = ociparse($db, "SELECT TO_CHAR(poczatek, 'YYYYMMDDHH24MI') FROM mm189188.nagrania WHERE nr=$sessionid");
  ociexecute($query);
  ocifetchinto($query, $arr);
  return timestampDate($arr[0])." ".timestampTime($arr[1]);
}

///////////////////////////////////////////////////
// Koniec sesji

function sessionEnd($sessionid)
{
  global $db;
  $query = ociparse($db, "SELECT TO_CHAR(koniec, 'YYYYMMDDHH24MI') FROM mm189188.nagrania WHERE nr=$sessionid");
  ociexecute($query);
  ocifetchinto($query, $arr);
  return timestampDate($arr[0])." ".timestampTime($arr[1]);
}
?>
