<? 
/*************************************************************************\
* STUDIO NAGRANIOWE                                                       *
* projekt zaliczeniowy z BD LAB                                           *
* autor: Maciek Makowski (mm189188)                                       *
*                                                                         *
* pytanie o potwierdzenie anulowania sesji                                *
\*************************************************************************/

  require "private/time.php3";
  require "private/db.php3";
  require "private/framework.php3";
  
  $db = ocilogon(_USER_, _PASSWORD_, _DB_);
  
  if (!logged($klient))
  {
    errorMessage("$klient nie jest zalogowany.");
    exit;
  }

  frameworkHeader();
  
  $query = ociparse($db, "SELECT TO_CHAR(poczatek, 'YYYYMMDDHH24MI'), TO_CHAR(koniec, 'YYYYMMDDHH24MI'), nazwa, zaplacone FROM mm189188.nagrania WHERE nr=$sesja");
  ociexecute($query);
  ocifetchinto($query, $arr);
    
?>
  <table width=100%>
    <tr>
      <td>
        <b>sesja:</b>
      </td>
      <td>
        <? echo $arr[2]; ?>
      </td>
    </tr>
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
        <b>czas trwania:</b>
      </td>
      <td>
        <? echo ((unixTimestamp($arr[1]) - unixTimestamp($arr[0])) / 3600) ." h"; ?>
      </td>
    </tr>
    <tr>
      <td>
        <b>koszt:</b>
      </td>
      <td>
        <? echo sessionCost($sesja)." z³"; ?>
      </td>
    </tr>
    <tr>
      <td>
        <b>zap³acono:</b>
      </td>
      <td>
        <? echo $arr[3]." z³"; ?>
      </td>
    </tr>
    <tr height=10>
      <td colspan=2>
        <hr size=1 width=75%>
      </td>
    </tr>
    <tr>
      <td valign=top>
        <b>Uwaga:</b>
      </td>
      <td>
        W przypadku anulowania sesji wp³acona dotychczas kwota <br>
        zgodnie z regulaminem Studia Nagraniowego nie podlega <br>
        zwrotowi.</div>
      </td>  
    </tr>
  </table>
  <br>
  <div class="warn" align=center><b>Czy napewno chcesz anulowaæ tê sesjê?</b></div>
  <br>
  <table>
    <tr>
      <td>
        <? button("Tak", "$nadawca.php3?klient=$klient&tryb=1&akcja=1&sesja=$sesja", 0, 60); ?>
      </td>
      <td>
        &nbsp;
      </td>
      <td>
        <? button("Nie", "$nadawca.php3?klient=$klient&tryb=0&sesja=$sesja", 0, 60); ?>
      </td>
      <td width=60%>      
        &nbsp;
      </td>
    </tr>
  </table>
<?
  
  frameworkFooter();


?>
