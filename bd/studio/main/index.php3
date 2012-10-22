<?
/*************************************************************************\
* STUDIO NAGRANIOWE                                                       *
* projekt zaliczeniowy z BD LAB                                           *
* autor: Maciek Makowski (mm189188)                                       *
*                                                                         *
* domyslna strona systemu - logowanie lub rejestracja                     *
\*************************************************************************/

  require "private/framework.php3";
  
  frameworkHeader();
?>
  <form action="main.php3?akcja=2" method="post">
    <table cellpadding=2>
      <tr>
        <td colspan=2 bgcolor=#dddddd>
          <div class=small><i>przyk³adowy klient:<br> u¿ytkownik: <b>scott</b><br> has³o: <b>tiger</b></i></div>
        </td>
      </tr>
      <tr>
        <td>
          u¿ytkownik:
        </td>
        <td>
          <input type=text width=20 name=uzytkownik>
        </td>
      </tr>
      <tr>
        <td>
          has³o:
        </td>
        <td>
          <input type=password width=20 name=haslo>
        </td>
      </tr>
    </table>
    <center>
      <input type=image src="gfx/loguj.gif" border=0>
    </center>
  </form>
  <center>
<?
  button("Nowy klient", "klient_nowy.php3");
?>
  </center>
<?
  frameworkFooter();
?>


