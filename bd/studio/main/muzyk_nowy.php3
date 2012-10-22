<?
/*************************************************************************\
* STUDIO NAGRANIOWE                                                       *
* projekt zaliczeniowy z BD LAB                                           *
* autor: Maciek Makowski (mm189188)                                       *
*                                                                         *
* formularz zgloszenia nowego muzyka                                      *
\*************************************************************************/

  require "private/framework.php3";
  require "private/time.php3";  
  require "private/db.php3";  

  $db = ocilogon(_USER_, _PASSWORD_, _DB_);

  if (!logged($klient))
  {
    errorMessage("$klient nie jest zalogowany.");
    exit;
  }

  switch ($akcja)
  {
    case 7:
      if (!$muz_imie) $errmsg = "nie poda³e¶ imienia muzyka.";
      elseif (!$muz_nazwisko) $errmsg = "nie poda³e¶ nazwiska muzyka.";
      elseif (!$muz_nrdok) $errmsg = "nie poda³e¶ numeru dokumentu muzyka.";
      elseif (!$muz_rodzdok) $errmsg = "nie podale¶ rodzaju dokumentu.";
      if ($errmsg) break;

      $mn = newMusician($muz_imie, $muz_nazwisko, $muz_nrdok, $muz_rodzdok);
      if ($mn)
      {
        frameworkHeader();
        ?>
        <center>Muzyk zosta³ dodany.<br>
        <?        
        button("OK", "$nadawca.php3?sesja=$sesja&klient=$klient&tryb=0&akcja=3&muzyk=$mn", 0, 80);
        frameworkFooter();
        exit;
      }
      else
        $errmsg = "nie uda³o siê dodaæ muzyka. Prawdopodobnie jest ju¿ zarejestrowany muzyk o tym imieniu i nazwisku.";
      break;
  }
  
  frameworkHeader();
?>
  <div class=nagl2>Zg³oszenie nowego muzyka</div>
  <form action="muzyk_nowy.php3?akcja=7&nadawca=<?echo $nadawca;?>&klient=<?echo $klient;?>&sesja=<?echo $sesja;?>" method="post">
<?
  if ($errmsg) echo "<div class=warn><b>B³±d:</b> $errmsg</div>";
?>  
    <table cellpadding=2>
      <tr>
        <td>
          <b>imiê:</b>
        </td>
        <td>
          <input type=text size=26 name=muz_imie value="<? echo $muz_imie; ?>">
        </td>
      </tr>
      <tr>
        <td>
          <b>nazwisko:</b>
        </td>
        <td>
          <input type=text size=26 name=muz_nazwisko value="<? echo $muz_nazwisko; ?>">
        </td>
      </tr>
      <tr>
        <td>
          <b>numer dokumentu:</b>
        </td>
        <td>
          <input type=text size=26 name=muz_nrdok value="<? echo $muz_nrdok; ?>">
        </td>
      </tr>
      <tr>
        <td>
          <b>rodzaj dokumentu:</b>
        </td>
        <td>
          <select name=muz_rodzdok>
<?
            $query = ociparse($db, "SELECT nazwa from mm189188.rodzaje_dokumentow
                                    ORDER BY nazwa");
            ociexecute($query);
            while(ocifetchinto($query, $arr1))
            {
              if ($arr1[0] == $muz_rodzdok) $d = "selected"; else $d = "";
              echo "<option $d value=\"".$arr1[0]."\">".$arr1[0];
            }
?>
          </select>
        </td>
      </tr>
    </table>
    <center>
      <input type=image src="gfx/dodaj.gif" border=0>
    </center>
  </form>
  <table>
    <tr>
      <td>
        <?button("Powrót", "$nadawca.php3?sesja=$sesja&klient=$klient&tryb=0", 0, 70);?>
      </td>
    </td>
  </table>
<?

  frameworkFooter();
?>

