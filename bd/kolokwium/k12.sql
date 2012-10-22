select numProj, COUNT(numP) as suma 
from (
select distinct numProj, numP from pracuje)
group by numProj
union
select proj.numProj, 0
from proj, pracuje
where not exists (select * from pracuje where pracuje.numProj = proj.numProj);