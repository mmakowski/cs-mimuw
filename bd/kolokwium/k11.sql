select numDz, nazwaDz, rozpietosc
from (
select numDz, nazwaDz, MAX(pensja) - MIN(pensja) as rozpietosc
from (
select dzialy.numDz, nazwaDz, pensja
from dzialy, prac
where prac.numDz = dzialy.numDz)
group by numDz, nazwaDz) tmp
where rozpietosc = (select MAX(rozpietosc) from tmp);
