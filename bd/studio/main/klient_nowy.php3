<?
/*************************************************************************\
* STUDIO NAGRANIOWE                                                       *
* projekt zaliczeniowy z BD LAB                                           *
* autor: Maciek Makowski (mm189188)                                       *
*                                                                         *
* formularz zgloszenia nowego klienta                                     *
\*************************************************************************/

  require "private/framework.php3";
  require "private/time.php3";  
  require "private/db.php3";  

  switch ($akcja)
  {
    case 6:
      $db = ocilogon(_USER_, _PASSWORD_, _DB_);

      if (!$kl_nazwa) $errmsg = "nie poda�e� nazwy klienta.";
      elseif (!($kl_nip_1 && $kl_nip_2 && $kl_nip_3 && $kl_nip_4)) $errmsg = "nie poda�e� numeru identyfikacji podatkowej<br> klienta.";
      elseif (!$kl_adres) $errmsg = "nie poda�e� adresu klienta.";
      elseif (!$kl_email) $errmsg = "nie poda�e� emailu klienta.";
      elseif (!$kl_login) $errmsg = "nie poda�e� loginu klienta.";
      elseif (!validLogin($kl_login)) $errmsg = "ten login jest ju� zaj�ty.";
      if ($errmsg) break;
      $kl_nip = $kl_nip_1."-".$kl_nip_2."-".$kl_nip_3."-".$kl_nip_4;
      if(addUser($kl_login, $kl_nazwa, $kl_nip, $kl_adres, $kl_email, $kl_telefon, $kl_fax))
        message("Klient zosta� wst�pnie zarejestrowany. Po weryfikacji zostanie wys�ane na podany email potwierdzenie, zawieraj�ce has�o dost�pu. <br> Dzi�kujemy za rejestracj�.");
      else
        errorMessage("Wyst�pi� b��d podczas rejestracji");
      exit;
      break;
  }
  
  frameworkHeader();
?>
  <div class=nagl2>Zg�oszenie nowego klienta</div>
  <form action="klient_nowy.php3?akcja=6" method="post">
<?
  if ($errmsg) echo "<div class=warn><b>B��d:</b> $errmsg</div>";
?>  
    <table cellpadding=2>
      <tr>
        <td>
          <b>nazwa:</b>
        </td>
        <td>
          <input type=text size=26 name=kl_nazwa value="<? echo $kl_nazwa; ?>">
        </td>
      </tr>
      <tr>
        <td>
          <b>NIP:</b>
        </td>
        <td>
          <input type=text size=2 maxlength=3 name=kl_nip_1 value="<? echo $kl_nip_1; ?>"> -
          <input type=text size=2 maxlength=3 name=kl_nip_2 value="<? echo $kl_nip_2; ?>"> -
          <input type=text size=1 maxlength=2 name=kl_nip_3 value="<? echo $kl_nip_3; ?>"> -
          <input type=text size=1 maxlength=2 name=kl_nip_4 value="<? echo $kl_nip_4; ?>">
        </td>
      </tr>
      <tr>
        <td>
          <b>adres:</b>
        </td>
        <td>
          <textarea name=kl_adres cols=30><?echo $kl_adres; ?></textarea>
        </td>
      </tr>
      <tr>
        <td>
          <b>email:</b>
        </td>
        <td>
          <input type=text size=26 name=kl_email value=<? echo $kl_email; ?>>
        </td>
      </tr>
      <tr>
        <td>
          telefon:
        </td>
        <td>
          <input type=text size=26 name=kl_telefon value="<? echo $kl_telefon; ?>">
        </td>
      </tr>
      <tr>
        <td>
          fax:
        </td>
        <td>
          <input type=text size=26 name=kl_fax value="<? echo $kl_fax; ?>">
        </td>
      </tr>
      <tr>
        <td>
          <b>login:</b>
        </td>
        <td>
          <input type=text size=26 name=kl_login value="<? echo $kl_login?>">
        </td>
      </tr>
    </table>
    <center>
      <input type=image src="gfx/dodaj.gif" border=0>
    </center>
  </form>
<?
  frameworkFooter();
?>

