select numDz, nazwaDz, COUNT(nrPok) from(
 select distinct dzialy.numDz, dzialy.nazwaDz, prac.nrPok from dzialy, prac
 where dzialy.numDz = prac.numDz)
group by numDz, nazwaDz;
