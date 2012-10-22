CREATE OR REPLACE TRIGGER miejsca_insert
AFTER INSERT ON miejsca
DECLARE
 CURSOR cimp from 
 i INTEGER;
BEGIN
 SELECT COUNT(*) FROM miejsca WHERE nr_imprezy = nr_imprezy AND miejsce <  INTO i;
 if i <> :NEW.miejsce - 1 then
 RAISE_APPLICATION_ERROR(-23456, 'Zly numer miejsca');
 end if;
END;

.
run

show errors trigger miejsca_insert;