select rola from pracuje where
not exists (select prac.numP from prac, dzialy where prac.NumP = dzialy.numKier AND
pracuje.numP = prac.numP);
