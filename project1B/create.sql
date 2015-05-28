CREATE TABLE Movie(
	id int,
	title varchar(100) not null,
	year int not null,
	rating varchar(10),
	company varchar(50) not null,
	primary key (id)
)ENGINE = INNODB;

CREATE TABLE Actor(
	id int,
	last varchar(20) not null,
	first varchar(20) not null,
	sex varchar(6) not null,
	dob date not null,
	dod date,
	primary key(id)
)ENGINE = INNODB;

CREATE TABLE Director(
	id int,
	last varchar(20) not null,
	first varchar(20) not null,
	dob date not null,
	dod date,
	primary key(id)
)ENGINE = INNODB;

CREATE TABLE MovieGenre(
	mid int,
	genre varchar(20),
	foreign key(mid) references Movie(id)
)ENGINE = INNODB;

CREATE TABLE MovieDirector(
	mid int,
	did int,
	foreign key(mid) references Movie(id),
	foreign key(did) references Director(id)
)ENGINE = INNODB;

CREATE TABLE MovieActor(
	mid int,
	aid int,
	role varchar(50) not null,
	foreign key(mid) references Movie(id),
	foreign key(aid) references Actor(id)
)ENGINE = INNODB;

CREATE TABLE Review(
	name varchar(20) not null,
	time timestamp not null,
	mid int,
	rating int not null,
	comment varchar(500),
	foreign key(mid) references Movie(id)
)ENGINE = INNODB;

CREATE TABLE MaxPersonID(
	id int
)ENGINE = INNODB;

CREATE TABLE MaxMovieID(
	id int
)ENGINE = INNODB;









