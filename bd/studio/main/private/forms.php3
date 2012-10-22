<?
/*************************************************************************\
* STUDIO NAGRANIOWE                                                       *
* projekt zaliczeniowy z BD LAB                                           *
* autor: Maciek Makowski (mm189188)                                       *
*                                                                         *
* formularze klienta                                                      *
\*************************************************************************/

///////////////////////////////////////////////////
// Informacje o sesji
function formSessionDetails()
{
  global $db, $klient, $muzyk, $sesja, $tryb, $nowa;

  frameworkHeader();
  
  $query = ociparse($db, "SELECT TO_CHAR(poczatek, 'YYYYMMDDHH24MI'), TO_CHAR(koniec, 'YYYYMMDDHH24MI'), nazwa, rabat, anulowane, zaplacone, studio, realizator FROM mm189188.nagrania WHERE nr=$sesja");
  ociexecute($query);
  ocifetchinto($query, $arr);
    
?>
  <div class=nagl2><? echo $arr[2] ?></div>
<?
  if($arr[4] == 'TAK')
  {
    ?><div class=warn align=center><b>(ANULOWANA)</b></div><?
  }
?>  
  <form action="sesja.php3?akcja=3&klient=<?echo $klient;?>&sesja=<?echo $sesja;?>&tryb=<?echo $tryb;?>" method="post">

  <hr size=1 width=100%>

  <table width=100%>
    <tr>
      <td>
        <b>pocz±tek:</b>
      </td>
      <td>
        <? echo timestampDate($arr[0])." ".timestampTime($arr[0]); ?>
      </td>
    </tr>
    <tr>
      <td>
        <b>koniec:</b>
      </td>
      <td>
        <? echo timestampDate($arr[1])." ".timestampTime($arr[1]); ?>
      </td>
    </tr>
    <tr>
      <td>
        <b>czas trwania:</b>
      </td>
      <td>
<?      
        $t = ((unixTimestamp($arr[1]) - unixTimestamp($arr[0])) / 3600);
        echo $t." h"; 
?>
      </td>
    </tr>
<?
    $query = ociparse($db, "SELECT imie, nazwisko, stawka, nr from mm189188.technicy, mm189188.technik_nagranie 
                                  WHERE technicy.nr = technik_nagranie.nr_technika AND technik_nagranie.nr_nagrania=$sesja
                                  ORDER BY nazwisko");
    ociexecute($query);
    if (ocifetchinto($query, $arr1))
    {
?>
    <tr>
      <td valign=top>
        <b>technicy:</b>
      </td>
      <td>
        <table cellpadding=0 cellspacing=0 width=100%>
<?  
        do
        {
          echo "<tr><td width=80%>".$arr1[0]." ".$arr1[1]." (".$arr1[2]." z³/h)";
          if (tryb == 0) echo "</td><td>&nbsp;</td><td width=15%><a href=\"sesja.php3?akcja=10&sesja=$sesja&klient=$klient&technik=".$arr1[3]."&tryb=0\" class=warnlink>zwolnij</a>";
          echo "</td></tr>";
        } while(ocifetchinto($query, $arr1));
?>
        </table>
      </td>
    </tr>
<?
    }
    $query = ociparse($db, "SELECT rodzaj, producent, model, stawka, COUNT(nr_seryjny) from mm189188.urzadzenia, mm189188.urzadzenie_nagranie 
                                  WHERE urzadzenia.nr = urzadzenie_nagranie.nr_urzadzenia AND urzadzenie_nagranie.nr_nagrania=$sesja
                                  GROUP BY rodzaj, producent, model, stawka ORDER BY rodzaj");
    ociexecute($query);
    if (ocifetchinto($query, $arr1))
    {
?>
    <tr>
      <td valign=top>
        <b>urz±dzenia:</b>
      </td>
      <td>
        <table cellpadding=0 cellspacing=0 width=100%>
<?  
        do
        {
          if (tryb == 0)
          {
            $q1 = ociparse($db, "SELECT nr from mm189188.urzadzenia, mm189188.urzadzenie_nagranie 
                                  WHERE urzadzenia.nr = urzadzenie_nagranie.nr_urzadzenia AND urzadzenie_nagranie.nr_nagrania=$sesja
                                  AND rodzaj='".$arr1[0]."' AND producent='".$arr1[1]."' AND model='".$arr1[2]."' AND stawka=".$arr1[3]);
            ociexecute($q1);
            ocifetchinto($q1, $arr2);
          }  
          $s = "<tr><td width=80%>".$arr1[0]." ".$arr1[1]." ".$arr1[2]." (".$arr1[3]." z³/h)";
          if ((int)$arr1[4] > 1) $s .= " x ".$arr1[4]." szt.";
          echo $s;
          if (tryb == 0) echo "</td><td>&nbsp;</td><td width=15%><a href=\"sesja.php3?akcja=11&sesja=$sesja&klient=$klient&urzadzenie=".$arr2[0]."&tryb=0\" class=warnlink>zwolnij</a>";
          echo "</td></tr>";
        } while(ocifetchinto($query, $arr1));
?>
        </table>
      </td>
    </tr>
<?
    }
?>
    <tr>
      <td>
        <b>studio:</b>
      </td>
      <td>
<? 
        $query = ociparse($db, "SELECT stawka from mm189188.studia WHERE nazwa='".$arr[6]."'");
        ociexecute($query);
        ocifetchinto($query, $arr1);
        echo $arr[6]." (".$arr1[0]." z³/h)";
?>
      </td>
    </tr>
    <tr>
      <td>
        <b>realizator:</b>
      </td>
      <td>
<? 
        if ($arr[7])
        {
          $query = ociparse($db, "SELECT imie, nazwisko, stawka from mm189188.realizatorzy WHERE nr=".$arr[7]);
          ociexecute($query);
          ocifetchinto($query, $arr1);
          echo "<table cellpadding=0 cellpsacing=0 width=100%><tr><td width=80%>".$arr1[0]." ".$arr1[1]." (".$arr1[2]." z³/h)</td><td>&nbsp</td><td width=15%><a href=\"sesja.php3?akcja=12&sesja=$sesja&klient=$klient&tryb=0\" class=warnlink>zwolnij</a></td></tr></table>";
        }
        else 
          echo "w³asny";
?>
      </td>
    </tr>
    <tr>
      <td colspan=2>
        <hr width=100% size=1>
      </td>
    </tr>
    <tr>
      <td valign=top>
        <b>rozliczenie:</b>
      </td>
      <td>
<? 
        $query = ociparse($db, "SELECT stawka from mm189188.studia WHERE nazwa='".$arr[6]."'");
        ociexecute($query);
        ocifetchinto($query, $arr1);
?>
        <table>
<?
          $k1 = personnelCostPerHour($sesja);
          if ($k1)
          {
?>
          <tr>
            <td>
              &nbsp;
            </td>
            <td>
              personel techniczny
            </td>
            <td>
              <? echo $k1." z³/h"; ?>
            </td>
          </tr>
<?
          }

          $k2 = devicesCostPerHour($sesja);
          if ($k2)
          {
?>
          <tr>
            <td>
              <? if($k1) echo "+"; else echo "&nbsp;"; ?>
            </td>
            <td>
              urz±dzenia
            </td>
            <td>
              <? echo $k2." z³/h"; ?>
            </td>
          </tr>
<?
          }

          $k3 = studioCostPerHour($sesja);
?>
          <tr>
            <td>
              <? if($k1 || $k2) echo "+"; else echo "&nbsp;"; ?>
            </td>
            <td>
              studio
            </td>
            <td>
              <? echo $k3." z³/h"; ?>
            </td>
          </tr>
          <tr>
            <td>
              &nbsp;
            </td>
            <td colspan=2>
              <hr width=100% size=1>
            </td>
          </tr>
          <tr>
            <td>
              =
            </td>
            <td>
              &nbsp;              
            </td>
            <td>
              <? echo $k1 + $k2 + $k3." z³/h"; ?>
            </td>
          </tr>
          <tr>
            <td>
              *
            </td>
            <td>
              &nbsp;
            </td>
            <td>
              <? echo ceil($t)." h"; ?>
            </td>
          </tr>
          <tr>
            <td>
              &nbsp;
            </td>
            <td colspan=2>
              <hr width=100% size=1>
            </td>
          </tr>
          <tr>
            <td>
              =
            </td>
            <td>
              &nbsp;              
            </td>
            <td>
              <? echo ($k1 + $k2 + $k3)*ceil($t)." z³"; ?>
            </td>
          </tr>
<?
          if((double)$arr[3])
          {
?>
          <tr>
            <td>
              -
            </td>
            <td>
              rabat
            </td>
            <td>
              <? echo $arr[3]." z³"; ?>
            </td>
          </tr>
          <tr>
            <td>
              &nbsp;
            </td>
            <td colspan=2>
              <hr width=100% size=1>
            </td>
          </tr>
          <tr>
            <td>
              =
            </td>
            <td>
              &nbsp;              
            </td>
            <td>
              <? echo (($k1 + $k2 + $k3)*ceil($t) - (double)$arr[3])." z³"; ?>
            </td>
          </tr>
<?
          }
?>
        </table>
      </td>
    </tr>
<?  
    if (!$nowa)
    {
?>
    <tr>
      <td>
        <b>zap³acono:</b>
      </td>
      <td>
        <? echo $arr[5]." z³"; ?>
      </td>
    </tr>
<?
    }
    if ($arr[4] == 'NIE')
    {
?>
    <tr>
      <td>
        <b>pozosta³o do sp³aty:</b>
      </td>
      <td>
        <? echo (($k1 + $k2 + $k3)*ceil($t) - (double)$arr[3] - (double)$arr[5])." z³"; ?>
      </td>
    </tr>
<?
    }
    if ($arr[4] == 'NIE')
    {
?>
    <tr>
      <td colspan=2>
        <hr width=100% size=1>
      </td>
    </tr>
    <tr>
      <td valign=top>
        <b>muzycy:</b>
      </td>
      <td>
        <table>
<?  
        $query = ociparse($db, "SELECT nr, imie, nazwisko from mm189188.muzycy, mm189188.muzyk_nagranie 
                                  WHERE muzycy.nr = muzyk_nagranie.nr_muzyka AND muzyk_nagranie.nr_nagrania=$sesja
                                  ORDER BY nazwisko");
        ociexecute($query);
        while(ocifetchinto($query, $arr1))
        {
?>
          <tr>
            <td>
              <?echo $arr1[1]." ".$arr1[2];?>
            </td>
            <td>
<?
              if ($tryb == 0)
              {
                ?><a href="sesja.php3?akcja=4&muzyk=<?echo $arr1[0];?>&klient=<?echo $klient;?>&sesja=<?echo $sesja;?>&tryb=<?echo $tryb;?>" class=warnlink>usuñ</a><?
              }
              else echo "&nbsp;";           
?>            
            </td>
          </tr>
<?          
        }
        if ($tryb == 0)
        {
?>
          <tr>
            <td>
              <select name=muzyk>
<?
                $query = ociparse($db, "SELECT nr, imie, nazwisko from mm189188.muzycy
                                          ORDER BY nazwisko");
                ociexecute($query);
                while(ocifetchinto($query, $arr1))
                  echo "<option value=".$arr1[0].">".$arr1[1]." ".$arr1[2];
?>
              </select>
            </td>
            <td>
              <input type=image src="gfx/dodaj.gif" border=0>
            </td>
          </tr>
          <tr>
            <td>
              &nbsp;
            </td>
            <td>
              <a href="muzyk_nowy.php3?klient=<?echo $klient;?>&sesja=<?echo $sesja;?>&tryb=0&nadawca=sesja" class=link>dodaj innego</a>
            </td>
          </tr>
<?
        }        
?>
        </table>
      </td>
    </tr>
<?
    }
?>
  </table>
  </form>
<?
  if ($nowa)
  {
?>
  <table>
    <tr>
      <td>
        <?button("Zatwierd¼", "main.php3?klient=$klient&tryb=$tryb", 0, 70);?>
      </td>
      <td>
        &nbsp;
      </td>
      <td>
        <?button("Zrezygnuj", "main.php3?klient=$klient&tryb=0&sesja=$sesja&akcja=18", 0, 70);?>
      </td>
    </tr>
  </table>
<?

  }
  else
  {
?>
  <table>
    <tr>
      <td>
        <?button("Powrót", "main.php3?klient=$klient&tryb=$tryb", 0, 70);?>
      </td>
      <td>
        &nbsp;
      </td>
      <td>
<?
        if ($tryb == 0)
          button("Anuluj", "anuluj.php3?nadawca=sesja&klient=$klient&tryb=1&sesja=$sesja", 0, 70);
        else
          echo "&nbsp;";
?>
      </td>
    </tr>
  </table>
<?
  }
  frameworkFooter();

} 

///////////////////////////////////////////////////
// Nowa sesja 
function formRemoveFromSession()
{
  global $db, $klient, $sesja, $akcja, $tryb, $technik, $urzadzenie;

  frameworkHeader();
?>
  <table width=100%>
    <tr>
      <td>
        <b>
        <?
          switch ($akcja)
          {
            case 10: echo "technik:"; break;
            case 11: echo "urz±dzenie:"; break;
            case 12: echo "realizator:"; break;
          }
        ?>
        </b>
      </td>
      <td>
        <?
          switch ($akcja)
          {
            case 10:
              $query = ociparse($db, "SELECT imie, nazwisko, stawka from mm189188.technicy where nr=$technik"); 
              ociexecute($query);
              ocifetchinto($query, $arr1);
              echo $arr1[0]." ".$arr1[1]." (".$arr1[2]." z³/h)";
              break;
            case 11:
              $query = ociparse($db, "SELECT rodzaj, producent, model, stawka FROM mm189188.urzadzenia WHERE nr = $urzadzenie");
              ociexecute($query);
              ocifetchinto($query, $arr1);
              echo $arr1[0]." ".$arr1[1]." ".$arr1[2]." (".$arr1[3]." z³/h)";
              break;
            case 12:
              $query = ociparse($db, "SELECT imie, nazwisko, stawka from mm189188.realizatorzy, mm189188.nagrania WHERE realizatorzy.nr = nagrania.realizator AND nagrania.nr=$sesja");
              ociexecute($query);
              ocifetchinto($query, $arr1);
              echo $arr1[0]." ".$arr1[1]." (".$arr1[2]." z³/h)";
              break;
          }
        ?>
      </td>
    </tr>
  </table>
  <br>
  <div class="warn" align=center><b>Czy napewno chcesz zwolniæ?</b></div>
  <br>
  <table>
    <tr>
      <td>
        <? button("Tak", "sesja.php3?klient=$klient&tryb=0&akcja=".($akcja + 10)."&sesja=$sesja&technik=$technik&urzadzenie=$urzadzenie", 0, 60); ?>
      </td>
      <td>
        &nbsp;
      </td>
      <td>
        <? button("Nie", "sesja.php3?klient=$klient&tryb=0&sesja=$sesja", 0, 60); ?>
      </td>
      <td width=60%>      
        &nbsp;
      </td>
    </tr>
  </table>
<?
  
  frameworkFooter();
}

///////////////////////////////////////////////////
// Nowa sesja 
function formNewSession()
{
  global $db, $klient, $sesja, $s_studio, $s_real, $s_czas, $s_pocz_m, $s_pocz_d, $s_pocz_y, $s_pocz_h, $errmsg
;

  frameworkHeader();
?>
  <div class=nagl2>Zg³oszenie nowej sesji</div>
  <form action="nowa.php3?akcja=13&klient=<?echo $klient;?>&sesja=<?echo $sesja;?>" method="post">
<?
  if ($errmsg) echo "<div class=warn><b>B³±d:</b> $errmsg</div>";
?>  
    <table cellpadding=2>
      <tr>
        <td>
          <b>nazwa:</b>
        </td>
        <td>
          <input type=text size=26 name=s_nazwa value="<? echo $s_nazwa; ?>">
        </td>
      </tr>
      <tr>
        <td>
          <b>pocz±tek:</b>
        </td>
        <td>
          <select name=s_pocz_d>
          <?for($i = 1; $i < 32; $i++) {$i = doubleDigit($i); if ($i == $s_pocz_d) echo "<option selected value=\"$i\">$i"; else echo "<option value=\"$i\">$i";}?>
          </select>.<select name=s_pocz_m>
          <?for($i = 1; $i < 13; $i++) {$i = doubleDigit($i); if ($i == $s_pocz_m) echo "<option selected value=\"$i\">$i"; else echo "<option value=\"$i\">$i";} ?>
          </select>.<select name=s_pocz_y>
          <?for($i = 2001; $i < 2003; $i++) if ($i == $s_pocz_y) echo "<option selected value=$i>$i"; else echo "<option value=$i>$i"; ?>
          </select>
          &nbsp;
          <select name=s_pocz_h>
          <?for($i = 0; $i < 24; $i++) 
            {
              if (doubleDigit($i).":00" == $s_pocz_h)
                echo "<option selected value=\"".doubleDigit($i).":00\">".doubleDigit($i).":00";
              else
                echo "<option value=\"".doubleDigit($i).":00\">".doubleDigit($i).":00";
              if (doubleDigit($i).":30" == $s_pocz_h)
                echo "<option selected value=\"".doubleDigit($i).":30\">".doubleDigit($i).":30";
              else
                echo "<option value=\"".doubleDigit($i).":30\">".doubleDigit($i).":30";
            }
          ?>
          </select>
        </td>
      </tr>
      <tr>
        <td>
          <b>czas trwania:</b>
        </td>
        <td>
          <select name=s_czas>
          <?for($i = 0; $i < 12; $i++) 
            {
              if ($i == $s_czas)
                echo "<option selected value=$i>".$i.":00";
              else
                echo "<option value=$i>".$i.":00";
              if ($i + 0.5 == $s_czas)
                echo "<option selected value=$i.5>".$i.":30";
              else
                echo "<option value=$i.5>".$i.":30";
            }
            if (12 == $s_czas)
              echo "<option selected value=12>12:00";
            else
              echo "<option value=12>12:00";
          ?>
          </select>
        </td>
      </tr>
      <tr>      
        <td>
          <b>studio:</b>
        </td>
        <td>
          <select name=s_studio>
<?
            $query = ociparse($db, "SELECT nazwa, stawka from mm189188.studia
                                    ORDER BY nazwa");
            ociexecute($query);
            while(ocifetchinto($query, $arr1))
            {
              if ($arr1[0] == $s_studio) $d = "selected"; else $d = "";
              echo "<option $d value=\"".$arr1[0]."\">".$arr1[0]." (".$arr1[1]." zl/h)";
            }
?>
          </select>
        </td>
      </tr>
      <tr>      
        <td>
          <b>realizator:</b>
        </td>
        <td>
          <select name=s_real>
<?
            if ($s_real) $d = "selected"; else $d = "";
            echo "<option $d value=0>w³asny";
            
            $query = ociparse($db, "SELECT imie, nazwisko, stawka, nr from mm189188.realizatorzy
                                    ORDER BY nazwisko");
            ociexecute($query);
            while(ocifetchinto($query, $arr1))
            {
              if ($arr1[3] == $s_real) $d = "selected"; else $d = "";
              echo "<option $d value=".$arr1[3].">".$arr1[0]." ".$arr1[1]." (".$arr1[2]." zl/h)";
            }
?>
          </select>
        </td>
      </tr>
    </table>
    <center>
      <input type=image src="gfx/dalej.gif" border=0>
    </center>
  </form>
  <table>
    <tr>
      <td>
        <?button("Powrót", "main.php3?tryb=0&klient=$klient", 0, 70);?>
      </td>
    </tr>
  </table>
<?

  frameworkFooter();
}

///////////////////////////////////////////////////
// Wybor technikow                     
function formChooseTechnicians()
{
  global $db, $klient, $sesja, $errmsg, $technik;

  frameworkHeader();
  $query = ociparse($db, "SELECT nazwa FROM mm189188.nagrania where nr = $sesja");
  ociexecute($query);
  ocifetchinto($query, $arr1);
?>
  <div class=nagl2>Zg³oszenie nowej sesji: <?echo $arr1[0];?></div>
  <div align=center>wybierz techników</div>
  <form action="nowa.php3?akcja=14&klient=<?echo $klient;?>&sesja=<?echo $sesja;?>" method="post">
<?
  if ($errmsg) echo "<div class=warn><b>B³±d:</b> $errmsg</div>";
?>  
  <table width=100%>
<?  
        $query = ociparse($db, "SELECT nr, imie, nazwisko, stawka from mm189188.technicy, mm189188.technik_nagranie 
                                  WHERE technicy.nr = technik_nagranie.nr_technika AND technik_nagranie.nr_nagrania=$sesja
                                  ORDER BY nazwisko");
        ociexecute($query);
        while(ocifetchinto($query, $arr1))
        {
?>
          <tr>
            <td>
              <?echo $arr1[1]." ".$arr1[2]. " (".$arr1[3]." z³/h)";?>
            </td>
            <td>
<?
              if ($tryb == 0)
              {
                ?><a href="nowa.php3?akcja=15&technik=<?echo $arr1[0];?>&klient=<?echo $klient;?>&sesja=<?echo $sesja;?>" class=warnlink>zrezygnuj</a><?
              }
              else echo "&nbsp;";           
?>            
            </td>
          </tr>
<?
        }
?>
          <tr>
            <td>
              <select name=technik>
<?
                $query = ociparse($db, "SELECT nr, imie, nazwisko, stawka from mm189188.technicy
                                          ORDER BY nazwisko");
                ociexecute($query);
                while(ocifetchinto($query, $arr1))
                  echo "<option value=".$arr1[0].">".$arr1[1]." ".$arr1[2]." (".$arr1[3]." z³/h)";
?>
              </select>
            </td>
            <td>
              <input type=image src="gfx/dodaj.gif" border=0>
            </td>
          </tr>
  </table>
  </form>
  <table>
    <tr>
      <td>
        <?button("Dalej", "nowa.php3?klient=$klient&sesja=$sesja&akcja=16", 0, 70);?>
      </td>
      <td>
        &nbsp;
      </td>
      <td>
        <?button("Zakoñcz", "sesja.php3?tryb=0&klient=$klient&sesja=$sesja&nowa=1", 0, 70);?>
      </td>
    </tr>
  </table>
<?
  frameworkFooter();
}

///////////////////////////////////////////////////
// Wybor urzadzen
function formChooseDevices()
{
  global $db, $klient, $sesja, $errmsg, $urzadzenie;

  frameworkHeader();
  $query = ociparse($db, "SELECT nazwa FROM mm189188.nagrania where nr = $sesja");
  ociexecute($query);
  ocifetchinto($query, $arr1);
?>
  <div class=nagl2>Zg³oszenie nowej sesji: <?echo $arr1[0];?></div>
  <div align=center>wybierz urz±dzenia</div>
  <form action="nowa.php3?akcja=17&klient=<?echo $klient;?>&sesja=<?echo $sesja;?>" method="post">
<?
  if ($errmsg) echo "<div class=warn><b>B³±d:</b> $errmsg</div>";
?>  
  <table width=100%>
<?  
        $query = ociparse($db, "SELECT nr, rodzaj, producent, model, stawka from mm189188.urzadzenia, mm189188.urzadzenie_nagranie 
                                  WHERE urzadzenia.nr = urzadzenie_nagranie.nr_urzadzenia AND urzadzenie_nagranie.nr_nagrania=$sesja
                                  ORDER BY rodzaj, producent, model");
        ociexecute($query);
        while(ocifetchinto($query, $arr1))
        {
?>
          <tr>
            <td>
              <?echo $arr1[1]." ".$arr1[2]." ".$arr1[3]." (".$arr1[4]." z³/h)";?>
            </td>
            <td>
<?
              if ($tryb == 0)
              {
                ?><a href="nowa.php3?akcja=19&urzadzenie=<?echo $arr1[0];?>&klient=<?echo $klient;?>&sesja=<?echo $sesja;?>" class=warnlink>zrezygnuj</a><?
              }
              else echo "&nbsp;";           
?>            
            </td>
          </tr>
<?
        }
?>
          <tr>
            <td>
              <select name=urzadzenie>
<?
                $query = ociparse($db, "SELECT nr, rodzaj, producent, model, stawka from mm189188.urzadzenia
                                          ORDER BY rodzaj, producent, model");
                ociexecute($query);
                while(ocifetchinto($query, $arr1))
                  echo "<option value=".$arr1[0].">".$arr1[1]." ".$arr1[2]." ".$arr1[3]." (".$arr1[4]." z³/h)";
?>
              </select>
            </td>
            <td>
              <input type=image src="gfx/dodaj.gif" border=0>
            </td>
          </tr>
  </table>
  </form>
  <table>
    <tr>
      <td>
        <?button("Zakoñcz", "sesja.php3?tryb=0&klient=$klient&sesja=$sesja&nowa=1", 0, 70);?>
      </td>
    </tr>
  </table>
<?
  frameworkFooter();
}
?>
