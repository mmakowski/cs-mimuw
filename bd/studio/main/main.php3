<? 
/*************************************************************************\
* STUDIO NAGRANIOWE                                                       *
* projekt zaliczeniowy z BD LAB                                           *
* autor: Maciek Makowski (mm189188)                                       *
*                                                                         *
* strona glowna klienta                                                   *
\*************************************************************************/

  require "private/time.php3";
  require "private/db.php3";
  require "private/framework.php3";
  
  $db = ocilogon(_USER_, _PASSWORD_, _DB_);

  if ($akcja != 2 && !logged($klient))
  {
    errorMessage("$klient nie jest zalogowany.");
    exit;
  }

  switch($akcja)
  { 
    case 1: cancelSession($sesja); break;
    case 2: 
      if (logged($uzytkownik))
      {
        errorMessage("Logowanie nie powiod³o siê.");
        exit;
      }
      login($uzytkownik, $haslo);
      if (!logged($uzytkownik))
      {
        errorMessage("Logowanie nie powiod³o siê.");
        exit;
      }
      else
      {      
        $klient = $uzytkownik;
        $tryb = 0;
      }
      break; 
    case 5: 
      logout($klient);
      if (logged($klient))
      {
        errorMessage("Nie uda³o siê wylogowaæ. Cofnij siê do poprzedniej<br> strony i spróbuj jeszcze raz. Je¶li nie wylogujesz siê rêcznie,<br> zostaniesz wylogowany automatycznie w ci±gu 12 godzin.");
        exit;
      }
      else
      {      
        message("$klient zosta³ wylogowany. Dziêkujemy za skorzystanie z serwisu.");
        exit;
      }
      break; 
    case 18: //zrezygnuj z sesji
      rollbackSession($klient, $sesja);
      break;
  }


  frameworkHeader(0);

  if ($tryb <> 0)
    if(userSessionsCount($klient, 0) > 0)
      button("Zaplanowane sesje", "main.php3?klient=$klient&tryb=0");
    else
      button("Zaplanowane sesje", "", 2);
  else
    button("Zaplanowane sesje", "", 1);
?>
  </td><td>
<?  
  if ($tryb <> 1)
    if(userSessionsCount($klient, 1) > 0)
      button("Anulowane sesje", "main.php3?klient=$klient&tryb=1");
    else
      button("Anulowane sesje", "", 2);
  else
    button("Anulowane sesje", "", 1);
?>
  </td><td>
<?  
  if ($tryb <> 2)
    if(userSessionsCount($klient, 2) > 0)
      button("Historia sesji", "main.php3?klient=$klient&tryb=2");
    else
      button("Historia sesji", "", 2);
  else
    button("Historia sesji", "", 1);
?>
  </td><td width=40%>&nbsp;</td></tr><tr><td colspan=4>
<?
  frameHeader();
?>
  <table width=100% cellpadding=2 cellspacing=0>
    <!-- naglowek -->
		<tr>
  	  <td bgcolor=#e8e8e8 colspan=2>
	      <b>pocz±tek</b>
			</td>
			<td bgcolor=#e4e4e4>
			  &nbsp;
			</td>
 			<td bgcolor=#e0e0e0 colspan=2>
			  <b>koniec</b>
			</td>
			<td bgcolor=#dcdcdc>
			  &nbsp;
			</td>
 			<td bgcolor=#d8d8d8>
			  <b>nazwa</b>
			</td>
		  <td bgcolor=#d4d4d4>
		    &nbsp;
		  </td>
		  <td bgcolor=#d0d0d0>
		    <b>zap³acono</b>
		  </td>
		  <td bgcolor=#cccccc>
		    &nbsp;
		  </td>
		  <td bgcolor=#c8c8c8 colspan=3>
		    &nbsp;
		  </td>
		</tr>
  
<?
  //wypisywanie sesji
  $qs = "SELECT nr, TO_CHAR(poczatek, 'YYYYMMDDHH24MI'), TO_CHAR(koniec, 'YYYYMMDDHH24MI'), nazwa, zaplacone FROM mm189188.nagrania WHERE klient='$klient'";
  if ($tryb == 1) $qs .= " AND anulowane='TAK'";
  elseif ($tryb == 0) $qs .= " AND anulowane='NIE'";
  $qs .= " ORDER BY poczatek";

  $query = ociparse($db, $qs);
  ociexecute($query);
  
  while(ocifetchinto($query, $arr))
  {
    
    //todo: inaczej w oracle'u
    if ($tryb == 2) { if (unixTimestamp($arr[1]) >= time()) continue; }
    else if (unixTimestamp($arr[1]) < time()) continue;
?>
		<tr>
  	  <td>
	      <? echo timestampDate($arr[1]); ?>
			</td>
  	  <td>
	      <? echo timestampTime($arr[1]); ?>
			</td>
			<td>
			  &nbsp;
			</td>
  	  <td>
	      <? echo timestampDate($arr[2]); ?>
			</td>
  	  <td>
	      <? echo timestampTime($arr[2]); ?>
			</td>
			<td>
			  &nbsp;
			</td>
 			<td>
	      <? echo $arr[3]; ?>
			</td>
		  <td>
		    &nbsp;
		  </td>
		  <td>
		    <? 
		      $pp = (int)($arr[4] / sessionCost($arr[0]) * 100); 
		      $timeoff = (unixTimestamp($arr[1]) - time()) / 86400;
		      $cl = "ok";
		      if ($timeoff < 10)
		      {  if ($pp < 50) $cl = "warn";}
		      elseif ($timeoff < 31)
		      {  if ($pp < 10) $cl = "warn";}
		      echo "<div class=\"$cl\">$pp%</div>";
		    ?>
		  </td>
		  <td>
		    &nbsp;
		  </td>
		  <td>
        <a href="sesja.php3?klient=<? echo $klient; ?>&sesja=<? echo $arr[0]; ?>&tryb=<? echo $tryb ?>" class=link>szczegó³y</a>
		  </td>
		  <td>
		    &nbsp;
		  </td>
		  <td>
        <?
          if ($tryb == 0) 
          {
          ?>
            <a href="anuluj.php3?nadawca=main&klient=<? echo $klient; ?>&sesja=<? echo $arr[0]; ?>" class=warnlink>anuluj</a>
          <?
          }
        ?>
		  </td>
		</tr>
<?    
  }
?>
  </table>
<?
  if ($tryb == 0)
  {
?>
  <table width=100% cellpadding=2>
    <tr>
      <td width=100% align=center>
        <? button("Nowa sesja", "nowa.php3?klient=$klient"); ?>
      </td>
    </tr>
  </table>
<?
  }
  frameFooter();

?>
  <br>
  <table width=100% cellpadding=2>
    <tr>
      <td width=100% align=left>
        <? button("Wyloguj", "main.php3?klient=$klient&akcja=5"); ?>
      </td>
    </tr>
  </table>    
<?

  frameworkFooter();
?>
