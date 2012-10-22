drop table dzialy;
drop table prac;
drop table proj;
drop table pracuje;

create table dzialy (
numDz integer primary key not null,
numKier integer not null,
nazwaDz varchar(20) not null
);

create table prac (
numP integer primary key not null,
Nazw varchar(20) not null,
pensja integer not null,
nrPok integer not null,
numDz integer not null
);

create table proj (
numProj integer primary key not null,
nazwaPr varchar(20) not null,
budzet integer not null
);

create table pracuje (
numP integer  not null,
numProj integer not null,
rola varchar(20) not null,
premia integer not null,
primary key(numP, numProj, rola)
);


insert into dzialy values (0, 1, 'dzial1');
insert into dzialy values (1, 2, 'dzial2');
insert into dzialy values (2, 3, 'dzial3');
insert into dzialy values (3, 4, 'dzial4');

insert into prac values (1, 'prac1', 2000, 1, 0);
insert into prac values (2, 'prac2', 2430, 1, 1);
insert into prac values (3, 'prac3', 1000, 2, 2);
insert into prac values (4, 'prac4', 1200, 2, 3);
insert into prac values (5, 'prac5', 1400, 2, 0);
insert into prac values (6, 'prac6', 200, 3, 1);
insert into prac values (7, 'prac7', 12000, 3, 2);
insert into prac values (8, 'prac8', 20, 3, 3);
insert into prac values (9, 'prac9', 3200, 3, 0);
insert into prac values (10, 'prac10', 400, 4, 1);
insert into prac values (11, 'prac11', 200, 4, 2);
insert into prac values (12, 'prac12', 2200, 4, 3);

insert into proj values (0, 'proj1', 10000);
insert into proj values (1, 'proj2', 10040);
insert into proj values (2, 'proj3', 40000);
insert into proj values (3, 'proj4', 100);

insert into pracuje values (1, 0, 'rola1', 20);
insert into pracuje values (2, 0, 'rola2', 0);
insert into pracuje values (3, 0, 'rola3', 120);
insert into pracuje values (1, 1, 'rola4', 1220);
insert into pracuje values (4, 1, 'rola5', 2);
insert into pracuje values (5, 2, 'rola6', 30);
insert into pracuje values (8, 2, 'rola7', 40);
