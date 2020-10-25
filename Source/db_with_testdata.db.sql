BEGIN TRANSACTION;
DROP TABLE IF EXISTS `UserTable`;
CREATE TABLE IF NOT EXISTS `UserTable` (
	`UserName`	VARCHAR ( 50 ) NOT NULL UNIQUE,
	`Passwd`	VARCHAR ( 50 ) NOT NULL,
	`Permissions`	INT ( 5 ) NOT NULL,
	PRIMARY KEY(`UserName`)
);
INSERT INTO `UserTable` VALUES ('Welcome','83218ac34c1834c26781fe4bde918ee4',1);
INSERT INTO `UserTable` VALUES ('Test','0cbc6611f5540bd0809a388dc95a615b',2);
INSERT INTO `UserTable` VALUES ('Hello','1dec6901f5240bc1209a388dc95a615b',1);
DROP TABLE IF EXISTS `ClassTable`;
CREATE TABLE IF NOT EXISTS `ClassTable` (
	`className`	VARCHAR ( 10 ) NOT NULL UNIQUE,
	`grade`	INT ( 10 ),
	`subject`	VARCHAR ( 50 ),
	PRIMARY KEY(`className`)
);
INSERT INTO `ClassTable` VALUES ('5a',5,'');
INSERT INTO `ClassTable` VALUES ('5aa',5,'');
INSERT INTO `ClassTable` VALUES ('5ab',5,'');
INSERT INTO `ClassTable` VALUES ('5b',5,'');
INSERT INTO `ClassTable` VALUES ('10a',10,'');
INSERT INTO `ClassTable` VALUES ('MatheLK',12,'Mathe');
INSERT INTO `ClassTable` VALUES ('DeutschLK',12,'Deutsch');
INSERT INTO `ClassTable` VALUES ('EnglischLK',11,'Englisch');
INSERT INTO `ClassTable` VALUES ('/2Lat',12,'Latein');
INSERT INTO `ClassTable` VALUES ('$2Bio',12,'Biologie');
INSERT INTO `ClassTable` VALUES ('ZuEntfernen',12,'');
DROP TABLE IF EXISTS `StudentTable`;
CREATE TABLE IF NOT EXISTS `StudentTable` (
	`preName`	VARCHAR ( 50 ) NOT NULL,
	`surName`	VARCHAR ( 50 ) NOT NULL,
	`address`	VARCHAR ( 200 ),
	`dateOfBirth`	VARCHAR ( 50 ) NOT NULL,
	`comment`	VARCHAR ( 200 ),
	PRIMARY KEY(`preName`,`surName`,`dateOfBirth`)
);
INSERT INTO `StudentTable` VALUES ('Vorname1-5a','Nachname1-5a','Musterstr. 1, 86150 Augsburg','31.07.2007','TestKommentar');
INSERT INTO `StudentTable` VALUES ('Vorname2-5a','Nachname2-5a','Musterst. 2, 86150 Augsburg','29.08.2006','NeuerTest');
INSERT INTO `StudentTable` VALUES ('Vorname3-5a','Nachname3-5a',NULL,'13.03.2005',NULL);
INSERT INTO `StudentTable` VALUES ('Vorname5-5a','Nachname5-5a',NULL,'11.08.2005',NULL);
INSERT INTO `StudentTable` VALUES ('Vorname1-5b','Nachname1-5b','Musterstr. 3, 86150 Augsburg','19.11.2005',NULL);
INSERT INTO `StudentTable` VALUES ('Vorname2-5b','Nachname2-5b','Musterstr. 4, 86150 Augsburg','25.12.2006',NULL);
INSERT INTO `StudentTable` VALUES ('Vorname3-5b','Nachname3-5b',NULL,'01.01.2006',NULL);
INSERT INTO `StudentTable` VALUES ('VornameÖ-5b','NachnameÖ-5b',NULL,'02.02.2005','TestKommentar');
INSERT INTO `StudentTable` VALUES ('VornameÖ-5b','NachnameÖ-5b',NULL,'03.03.2006',NULL);
INSERT INTO `StudentTable` VALUES ('Vorname1-10a','Nachname1-10a','Musterstr. 1, 80331 München','04.05.2001',NULL);
INSERT INTO `StudentTable` VALUES ('Vorname2-10a','Nachname2-10a',NULL,'07.03.2000',NULL);
INSERT INTO `StudentTable` VALUES ('Vorname3-10a','Nachname3-10a',NULL,'02.08.2001',NULL);
DROP TABLE IF EXISTS `StudentClassTable`;
CREATE TABLE IF NOT EXISTS `StudentClassTable` (
	`studentPreName`	VARCHAR ( 50 ) NOT NULL,
	`studentSurName`	VARCHAR ( 50 ) NOT NULL,
	`StudentDateOfBirth`	VARCHAR ( 50 ) NOT NULL,
	`class`	VARCHAR ( 50 ) NOT NULL,
	FOREIGN KEY(`class`) REFERENCES `ClassTable`(`className`),
	FOREIGN KEY(`studentPreName`,`studentSurName`,`StudentDateOfBirth`) REFERENCES `StudentTable`(`preName`,`surName`,`dateOfBirth`),
	PRIMARY KEY(`studentPreName`,`studentSurName`,`StudentDateOfBirth`,`class`)
);
INSERT INTO `StudentClassTable` VALUES ('Vorname1-5a','Nachname1-5a','31.07.2007','5a');
INSERT INTO `StudentClassTable` VALUES ('Vorname1-5a','Nachname1-5a','31.07.2007','5ab');
INSERT INTO `StudentClassTable` VALUES ('Vorname2-5a','Nachname2-5a','29.08.2006','5a');
INSERT INTO `StudentClassTable` VALUES ('Vorname3-5a','Nachname3-5a','13.03.2005','5a');
INSERT INTO `StudentClassTable` VALUES ('Vorname3-5a','Nachname3-5a','13.03.2005','5aa');
INSERT INTO `StudentClassTable` VALUES ('Vorname5-5a','Nachname5-5a','11.08.2005','5a');
INSERT INTO `StudentClassTable` VALUES ('Vorname1-5b','Nachname1-5b','19.11.2005','5b');
INSERT INTO `StudentClassTable` VALUES ('Vorname2-5b','Nachname2-5b','25.12.2006','5b');
INSERT INTO `StudentClassTable` VALUES ('Vorname2-5b','Nachname2-5b','25.12.2006','ZuEntfernen');
INSERT INTO `StudentClassTable` VALUES ('Vorname3-5b','Nachname3-5b','01.01.2006','5b');
INSERT INTO `StudentClassTable` VALUES ('Vorname3-5b','Nachname3-5b','01.01.2006','ZuEntfernen');
INSERT INTO `StudentClassTable` VALUES ('VornameÖ-5b','NachnameÖ-5b','02.02.2005','5b');
INSERT INTO `StudentClassTable` VALUES ('VornameÖ-5b','NachnameÖ-5b','03.03.2006','5b');
INSERT INTO `StudentClassTable` VALUES ('Vorname1-10a','Nachname1-10a','04.05.2001','MatheLK');
INSERT INTO `StudentClassTable` VALUES ('Vorname1-10a','Nachname1-10a','04.05.2001','DeutschLK');
INSERT INTO `StudentClassTable` VALUES ('Vorname1-10a','Nachname1-10a','04.05.2001','EnglischLK');
INSERT INTO `StudentClassTable` VALUES ('Vorname1-10a','Nachname1-10a','04.05.2001','10a');
INSERT INTO `StudentClassTable` VALUES ('Vorname2-10a','Nachname2-10a','07.03.2000','MatheLK');
INSERT INTO `StudentClassTable` VALUES ('Vorname2-10a','Nachname2-10a','07.03.2000','/2Lat');
INSERT INTO `StudentClassTable` VALUES ('Vorname2-10a','Nachname2-10a','07.03.2000','$2Bio');
INSERT INTO `StudentClassTable` VALUES ('Vorname2-10a','Nachname2-10a','07.03.2000','10a');
INSERT INTO `StudentClassTable` VALUES ('Vorname3-10a','Nachname3-10a','02.08.2001','EnglischLK');
DROP TABLE IF EXISTS `BookTable`;
CREATE TABLE IF NOT EXISTS `BookTable` (
	`isbn`	VARCHAR ( 20 ) NOT NULL UNIQUE,
	`title`	VARCHAR ( 50 ) NOT NULL,
	`subtitle`	VARCHAR ( 50 ),
	`publisher`	VARCHAR ( 50 ),
	`author`	VARCHAR ( 50 ) NOT NULL,
	`subject`	VARCHAR ( 50 ),
	`grade`	VARCHAR ( 50 ),
	`edition`	INT ( 10 ),
	`price`	VARCHAR ( 50 ),
	`stock`	INT ( 20 ) NOT NULL,
	`available`	INT ( 20 ) NOT NULL,
	`comment`	VARCHAR ( 100 ),
	`stocktakingCount`	INT ( 20 ),
	`stocktakingDate`	VARCHAR ( 50 ),
	PRIMARY KEY(`isbn`)
);
INSERT INTO `BookTable` VALUES ('975-1','Buch1','Unter1','Hans1','B1','Fach1','5,6',1,'1.00 €',11,11,'Kommentar1',1,'1.1.2000');
INSERT INTO `BookTable` VALUES ('975-2','Buch2','Unter2','Hans2','B2','Fach2','5',1,'2.00 €',11,11,'Kommentar2',1,'1.1.2000');
INSERT INTO `BookTable` VALUES ('975-3','Buch3','Unter3','Hans3','B3','Fach3','5',3,'3.00 €',33,33,'Kommentar3',3,'3.1.2000');
INSERT INTO `BookTable` VALUES ('975-4','Buch4','Unter4','Hans4','B4','Fach4','5',4,'4.00 €',44,44,'Kommentar4',4,'4.1.2000');
INSERT INTO `BookTable` VALUES ('975-5','Buch5','Unter5','Hans5','B5','Fach5','10',5,'5.00 €',55,55,'Kommentar5',5,'5.1.2000');
INSERT INTO `BookTable` VALUES ('975-6','Buch6','Unter6','Hans6','B6','Fach6','10',6,'6.00 €',66,66,'Kommentar6',6,'6.1.2000');
INSERT INTO `BookTable` VALUES ('975-7','Buch7','Unter7','Hans7','B7','Fach7','11',7,'7.00 €',77,77,'Kommentar7',7,'7.1.2000');
INSERT INTO `BookTable` VALUES ('975-8','Buch8','Unter8','Hans8','B8','Fach8','11',8,'8.00 €',88,88,'Kommentar8',8,'8.1.2000');
INSERT INTO `BookTable` VALUES ('975-9','Buch9','Unter9','Hans9','B9','Fach9','12',9,'9.00 €',99,99,'Kommentar9',9,'9.1.2000');
INSERT INTO `BookTable` VALUES ('975-10','Buch10','Unter10','Hans10','B10','Fach10','12',10,'10.00 €',100,100,'Kommentar10',10,'10.1.2000');
DROP TABLE IF EXISTS `ReturnTable`;
CREATE TABLE IF NOT EXISTS `ReturnTable` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`damage`	VARCHAR ( 50 ),
	`date`	VARCHAR ( 50 ) NOT NULL,
	`price`	VARCHAR ( 50 )
);
INSERT INTO `ReturnTable` VALUES (1,NULL,'20.10.2017','');
INSERT INTO `ReturnTable` VALUES (2,'Wasserschaden','20.10.2017','');
INSERT INTO `ReturnTable` VALUES (3,'Zerknittert','20.10.2017','9,99€');
INSERT INTO `ReturnTable` VALUES (4,'Wasserschaden','20.10.2017','');
DROP TABLE IF EXISTS `LendingTable`;
CREATE TABLE IF NOT EXISTS `LendingTable` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`studentPreName`	VARCHAR ( 50 ) NOT NULL,
	`studentSurName`	VARCHAR ( 50 ) NOT NULL,
	`StudentDateOfBirth`	VARCHAR ( 50 ) NOT NULL,
	`book`	VARCHAR ( 50 ) NOT NULL,
	`lendingType`	VARCHAR ( 50 ),
	`lendDate`	VARCHAR ( 50 ) NOT NULL,
	`bringBackDate`	VARCHAR ( 50 ),
	`returnID`	INTEGER,
	FOREIGN KEY(`studentPreName`,`studentSurName`,`StudentDateOfBirth`) REFERENCES `StudentTable`(`preName`,`surName`,`dateOfBirth`),
    FOREIGN KEY(`book`) REFERENCES `BookTable`(`isbn`)
);
INSERT INTO `LendingTable` VALUES (1,'Vorname1-5a','Nachname1-5a','31.07.2007','975-1',0,'12.09.2017','20.08.2018',0);
INSERT INTO `LendingTable` VALUES (2,'Vorname1-5a','Nachname1-5a','31.07.2007','975-2',1,'12.09.2017','20.10.2017',1);
INSERT INTO `LendingTable` VALUES (3,'Vorname1-10a','Nachname1-10a','04.05.2001','975-3',0,'12.09.2017','20.08.2018',0);
INSERT INTO `LendingTable` VALUES (4,'Vorname1-10a','Nachname1-10a','04.05.2001','975-1',1,'12.09.2017','20.10.2017',2);
INSERT INTO `LendingTable` VALUES (5,'Vorname1-5b','Nachname1-5b','19.11.2005','975-2',0,'12.09.2017','20.08.2018',3);
INSERT INTO `LendingTable` VALUES (6,'Vorname1-5b','Nachname1-5b','19.11.2005','975-3',1,'12.09.2017','20.10.2017',4);
COMMIT;
