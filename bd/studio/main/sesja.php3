<? 
/*************************************************************************\
* STUDIO NAGRANIOWE                                                       *
* projekt zaliczeniowy z BD LAB                                           *
* autor: Maciek Makowski (mm189188)                                       *
*                                                                         *
* szczegoly sesji                                                         *
\*************************************************************************/

  require "private/time.php3";
  require "private/db.php3";
  require "private/framework.php3";
  require "private/forms.php3";
  
  $db = ocilogon(_USER_, _PASSWORD_, _DB_);

  if (!logged($klient))
  {
    errorMessage("$klient nie jest zalogowany.");
    exit;
  }

  switch($akcja)
  {
    case 1: cancelSession($sesja); break;
    case 3: addMusician($sesja, $muzyk); break;
    case 4: removeMusician($sesja, $muzyk); break;
    case 10:
    case 11:
    case 12:
      formRemoveFromSession();
      exit;
      break;
    case 20: removeTechnician($sesja, $technik); break;
    case 21: removeDevice($sesja, $urzadzenie); break;
    case 22: removeEngineer($sesja); break;
  }

  formSessionDetails();

?>
