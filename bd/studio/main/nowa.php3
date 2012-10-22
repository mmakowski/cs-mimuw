<?
/*************************************************************************\
* STUDIO NAGRANIOWE                                                       *
* projekt zaliczeniowy z BD LAB                                           *
* autor: Maciek Makowski (mm189188)                                       *
*                                                                         *
* formularz zgloszenia nowej sesji                                        *
\*************************************************************************/

  require "private/framework.php3";
  require "private/time.php3";  
  require "private/db.php3";  
  require "private/forms.php3";

  $db = ocilogon(_USER_, _PASSWORD_, _DB_);

  if (!logged($klient))
  {
    errorMessage("$klient nie jest zalogowany.");
    exit;
  }

  switch ($akcja)
  {
    case 13: //rejestrowanie sesji i wyswietlenie formularza wyboru technikow
      $from = $s_pocz_d.".".$s_pocz_m.".".$s_pocz_y." ".$s_pocz_h;
      $fromts = unixTimestamp($s_pocz_y.$s_pocz_m.$s_pocz_d.$s_pocz_h[0].$s_pocz_h[1].$s_pocz_h[3].$s_pocz_h[4]);
      $to = date("d.m.Y H:i", $fromts + (int)($s_czas * 3600));
      if (!$s_nazwa) $errmsg = "nie poda³e¶ nazwy sesji.";
      elseif (!studioAvailable($s_studio, $from, $to)) $errmsg = "W podanym czasie studio $s_studio jest zajête.";
      elseif ($s_real && !engineerAvailable($s_real, $from, $to)) $errmsg = "W podanym czasie wybrany realizator jest zajêty.";
      if ($errmsg) break;
      $sesja = registerSession($klient, $s_nazwa, $s_studio, $s_real, $from, $to);
      if (!$sesja) $errmsg = "nie uda³o siê zarejestrowaæ sesji. Sprawd¼, czy napewno dobrze wpisa³e¶ datê rozpoczêcia.";
      else
      {
        formChooseTechnicians();
        exit;
      }
      break;
    case 14: //dodanie nowego technika 
      if (!technicianAvailable($technik, sessionStart($sesja), sessionEnd($sesja))) $errmsg = "W podanym czasie wybrany technik jest zajêty.";
      else addTechnician($sesja, $technik);
      formChooseTechnicians();
      exit;
      break;
    case 15: //rezygnacja z technika
      removeTechnician($sesja, $technik);
      formChooseTechnicians();
      exit;
      break;
    case 16: //wyswietlenie formularza wyboru urzadzen
      formChooseDevices();
      exit;
      break;
    case 17: //dodanie nowego urzadzenia
      if (!deviceAvailable($urzadzenie, sessionStart($sesja), sessionEnd($sesja))) $errmsg = "W podanym czasie wybrane urz±dzenie jest zajête.";
      else addDevice($sesja, $urzadzenie);
      formChooseDevices();
      exit;
      break;
    case 19: //rezygnacja z urzadzenia
      removeDevice($sesja, $urzadzenie);
      formChooseDevices();
      exit;
      break;
  }
  
  formNewSession();
?>
