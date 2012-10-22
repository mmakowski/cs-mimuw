select numP, nazw from prac, (
 select nrPok, COUNT(numDz) as rdz from(
  select distinct nrPok, numDz from prac)
 group by nrPok) a
where prac.nrPok = a.nrPok AND a.rdz < 4;