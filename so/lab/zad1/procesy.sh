#!/bin/bash

# SO LAB rozwiazanie zad. 1
# autor: Maciek Makowski (mm189188)
# 25.10.2001

# ============================== konfiguracja ================================

# liczba sekund czasu procesora powyzej ktorej proces jest podejrzany
SUSPICIOUS_TIME=1800

# lista uzytkownikow, ktorych procesy nas nie interesuja
IGNORE_USERS=""

# lista uzytkownikow, ktorych procesow nie bedziemy podejrzewali
IGNORE_SUSP_CHK="bin daemon nobody root"


# ================================= funkcje ==================================

# -- jest_podejrzany(usr, cpu, tty) ------------------------------------------
# decyduje, czy proces o zadanych parametrach jest podejrzany
# usr = wlasciciel procesu
# cpu = zuzyty czas procesora (w sekundach)
# tty = terminal, z ktorym proces jest zwiazany
function jest_podejrzany()
{
    if [ $# -le 2 ]; then 
	echo "Blad: zbyt malo parametrow dla funkcji jest_podejrzany()";
    fi

    local usr=$1 cpu=$2 tty=$3 s=0
    if [ $tty = "?" ]; then s=1; fi
    if [ $SUSPICIOUS_TIME -le $cpu ]; then s=1; fi
    if [ 1 -le `echo $IGNORE_SUSP_CHK | grep -c $usr` ]; then s=0; fi
    if [ $s -eq 1 ]; then true; else false; fi
}

# -- wypisz_stat_uz(usr, pcs, mem, cpu) --------------------------------------
# wypisuje na standardowe wyjscie informacje o procesach danego uzytkownika
# usr = nazwa uzytkownika
# pcs = liczba procesow
# mem = zajmowana ilosc pamieci
# cpu = zuzyty czas procesora
function wypisz_stat_uz()
{
    if [ $# -le 3 ]; then 
	echo "Blad: zbyt malo parametrow dla funkcji wypisz_stat_uz()";
    fi

    local usr=$1 pcs=$2 mem=$3 cpu=$4
    if [ `echo $IGNORE_USERS | grep -c $usr` -le 0 ]; then
	t=`expr length $usr`;
	d=$((8-$t));
	while [ 1 -le $d ]; do
	    usr="$usr ";
	    d=$(($d-1));
	done;
	echo -e "$usr\t$pcs\t$mem\t$cpu";
    fi
}

# -- powiadom(usr, msg) -----------------------------------------------------
# wysyla wiadomosc do uzytkownika - jesli jest zalogowany, to na terminal, 
# wpp poczta elektroniczna
# usr = nazwa uzytkownika
# msg = tresc wiadomosci
function powiadom()
{
    if [ $# -le 1 ]; then 
	echo "Blad: zbyt malo parametrow dla funkcji powiadom()";
    fi

    local usr=$1 msg=$2 tty=""
    if [ 1 -le `who | grep -c $usr` ]; then 
	tty=`who | grep $usr | uniq -w 8 | tr -s [:blank:] | cut -d " " -f 2`
	echo -e $msg | (write $usr $tty 2> /dev/null) || echo -e $msg > /dev/$tty;
    else
	echo -e $msg | mail $usr -s "Podejrzane procesy" > /dev/null;
    fi
}

# -- raport() ----------------------------------------------------------------
# dla kazdego uzytkownika: 1) wypisuje informacje o jego procesach; 
# 2) sprawdza, czy ktores z jego procesow nie sa podejrzane, i jesli sa
# powiadamia go o tym
# nie przyjmuje argumentow
# na standardowym wejsciu przyjmuje informacje o procesach w systemie 
# uzyskane (odpowiednio zformatowanym) poleceniem ps
function raport()
{
    local cusr="UZYTKOWNIK" cmem="PAMIEC" ccpu="CZAS (s)" cpcs="PROCESY" csus=0
    read usr mem cpu tty pid com
    while [ "x$usr" != "x" ]; do
	if [ $usr != $cusr ]; then 
	    wypisz_stat_uz "$cusr" "$cpcs" "$cmem" "$ccpu";
	    if [ 1 -le $csus ]; then
		powiadom $cusr "\n\rNie podobaja mi sie Twoje procesy:\n\r$cspl";
	    fi;
	    cusr=$usr; cpcs=0; cmem=0; ccpu=0; csus=0; 
	    cspl="PID\tTTY\tPAMIEC\tCZAS\tPOLECENIE";
	fi;
	if [ `echo $IGNORE_USERS | grep -c $usr` -le 0 ]; then
	    cpcs=$(($cpcs+1));
	    cmem=$(($cmem+$mem));
	    pcpu=$((`echo $cpu | sed s/:/*60+/`))
	    ccpu=$(($ccpu+$pcpu));
	    jest_podejrzany $usr $pcpu $tty && {
		csus=1;
		cspl="$cspl\n\r$pid\t$tty\t$mem\t$cpu\t$com";
	    };
	fi;
	read usr mem cpu tty pid com;
    done
    wypisz_stat_uz "$cusr" "$cpcs" "$cmem" "$ccpu"
    if [ 1 -le $csus ]; then
	powiadom $cusr "Nie podobaja mi sie Twoje procesy:\n\r$cspl"
    fi;
}

# ----------------------------------------------------------------------------
ps ax -o user,rssize,bsdtime,tty,pid,ucomm O u | tail +2 | raport 
