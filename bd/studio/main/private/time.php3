<?
/*************************************************************************\
* STUDIO NAGRANIOWE                                                       *
* projekt zaliczeniowy z BD LAB                                           *
* autor: Maciek Makowski (mm189188)                                       *
*                                                                         *
* plik z funkcjami obslugujacymi czas i daty                              *
\*************************************************************************/

///////////////////////////////////////////////////
// Przerabianie liczby na dwucyfrowa 

function doubleDigit($str)
{
  if (strlen((string)$str) == 1) return "0".((string)$str); else return (string)$str;
}

///////////////////////////////////////////////////
// Wyciaganie daty z timestampu 

function timestampDate($timestamp)
{
  $s = (string)$timestamp;
  return $s[6].$s[7].".".$s[4].$s[5].".".$s[0].$s[1].$s[2].$s[3];
}

///////////////////////////////////////////////////
// Wyciaganie czasu z timestampu 

function timestampTime($timestamp)
{
  $s = (string)$timestamp;
  return $s[8].$s[9].":".$s[10].$s[11];
}

///////////////////////////////////////////////////
// Tworzenie UNIX timestamp z timestampu 

function unixTimestamp($timestamp)
{
  $s = (string)$timestamp;
  return mktime((int)($s[8].$s[9]), (int)($s[10].$s[11]), 00, (int)($s[4].$s[5]), (int)($s[6].$s[7]), (int)($s[0].$s[1].$s[2].$s[3]));
}


