select numProj, nazwaPr from(
 select numProj, nazwaPr, MAX(proj) as maxpr from(
  select proj.numProj, proj.nazwaPr, proj as proj from proj, pracuje, (
   select distinct prac.numP, COUNT(numProj) as proj from prac, pracuje
   where prac.numP = pracuje.numP
   group by prac.numP) a
  where pracuje.numProj = proj.numProj AND pracuje.numP = a.numP)
 group by numProj, nazwaPr)
where maxpr < 2;