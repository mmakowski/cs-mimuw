<? 
/*************************************************************************\
* STUDIO NAGRANIOWE                                                       *
* projekt zaliczeniowy z BD LAB                                           *
* autor: Maciek Makowski (mm189188)                                       *
*                                                                         *
* szablon wygladu strony                                                  *
\*************************************************************************/

///////////////////////////////////////////////////
// Gora strony

function frameworkHeader($plain = 1)
{
  global $klient;
?>
<html>
<head>
  <TITLE>Studio nagraniowe</TITLE>
  <meta name="Author" content="Maciek Makowski - m.makowski@zodiac.mimuw.edu.pl">
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-2">
  <link href="styles/default.css" rel=stylesheet type=text/css>
</head>

<body>

  <center>
    <table border=0 cellpadding=0 cellspacing=1 bgcolor=#000000>
      <tr>
  	    <td bgcolor=#ffffff>
	    	  <table width=100% cellpadding=0 cellspacing=10>
<?
  if (!$plain)
  {
?>
<!-- pietra naglowka -->
            <tr>
  			      <td colspan=4 align=center>
	  		        <img src="gfx/studio.jpg">
		  	      </td>
		  	    </tr>
            <tr>
			        <td colspan=4>
			          <div class=nagl1><?echo userName($klient);?></div>
    			    </td>
    			  </tr>
<?
  }
?>
<!-- nastepne pietra -->
            <tr>
              <td>
<?
}

///////////////////////////////////////////////////
// Dol strony

function frameworkFooter()
{
?>
              </td>
            </tr>
		      </table>
        </td>
	    </tr>
    </table>
  </center>
  
</body>
</html>
<?
}

///////////////////////////////////////////////////
// Ramka - gora

function frameHeader($width = "100%")
{
?>
  <table width=<?echo "$width";?> cellpadding=0 cellspacing=1 bgcolor=#000000>
    <tr>
	    <td bgcolor=#ffffff>

<?
}

///////////////////////////////////////////////////
// Ramka - dol

function frameFooter()
{
?>
      </td>
    </tr>
  </table>
<?
}


///////////////////////////////////////////////////
// Przycisk

function button($text, $link = "", $state = 0, $width = 150)
{
  switch($state)
  {
   case 0: //normal
    $bordercolor = "#0044aa";
    $bgcolor = "#0066ee";
    $txtclass = "btnlink";
    break;
   case 1: //selected
    $bordercolor = "#0044aa";
    $bgcolor = "#0066ee";
    $txtclass = "btnsel";
    break;
   case 2: //disabled
    $bordercolor = "#333333";
    $bgcolor = "#888888";
    $txtclass = "btndis";
    break;
  }
?>
<table border=0 cellpadding=2 cellspacing=1 bgcolor=<?echo "$bordercolor";?>>
  <tr>
  	<td bgcolor=<?echo "$bgcolor";?> align=center width=<?echo "$width";?>>
<?
  switch($state)
  {
   case 0: //normal
?>
    <a href=<?echo "$link";?> class=<?echo "$txtclass";?>><?echo "$text";?></a>
<?
    break;
   case 1: //selected
   case 2: //disabled
?>
    <div class=<?echo "$txtclass";?>><?echo "$text";?></div>
<?
  }
?>
   	</td>
  <tr>
</table>
<?
}

///////////////////////////////////////////////////
// Inforamcja o bledzie

function errorMessage($msg)
{
  frameworkHeader();
?>
  <center>
    <div align=center class=warn><? echo $msg; ?></div>
  </center>
<?
  frameworkFooter();
}

///////////////////////////////////////////////////
// Inforamcja

function message($msg)
{
  frameworkHeader();
?>
  <center>
    <div align=center><? echo $msg; ?></div>
  </center>
<?
  frameworkFooter();
}
?>
