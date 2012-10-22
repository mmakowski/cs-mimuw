DROP PROCEDURE dofinal;

CREATE PROCEDURE dofinal AS 
  CURSOR czaw1 IS SELECT nazwa_zawodnika, suma FROM ranking ORDER BY suma DESC;
  CURSOR czaw2 IS SELECT nazwa_zawodnika FROM zawodnicy WHERE NOT EXISTS (SELECT *  FROM ranking WHERE ranking.nazwa_zawodnika = zawodnicy.nazwa_zawodnika);
  CURSOR czaw3 IS SELECT nazwa_zawodnika, ranking FROM final;
  zaw1 ranking%ROWTYPE;
  zaw2 zawodnicy%ROWTYPE;
  zaw3 final%ROWTYPE;
  i INTEGER;
BEGIN
 /* zrob tabele final */
  DELETE FROM final;
  i := 1;
  for zaw1 in czaw1 loop
    INSERT INTO final VALUES (i, zaw1.nazwa_zawodnika, zaw1.suma);
    i := i+1;
  end loop;
  for zaw2 in czaw2 loop
    INSERT INTO final VALUES (i, zaw2.nazwa_zawodnika, 0);
    i := i+1;
  end loop;
 /* zaktualizuj rankingi */
  for zaw3 in czaw3 loop
    UPDATE zawodnicy SET ranking = zaw3.ranking WHERE zawodnicy.nazwa_zawodnika = zaw3.nazwa_zawodnika;  
  end loop;
END;

.
run

show errors procedure dofinal;