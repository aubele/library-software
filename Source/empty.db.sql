BEGIN TRANSACTION;
DROP TABLE IF EXISTS `UserTable`;
CREATE TABLE IF NOT EXISTS `UserTable` (
	`UserName`	VARCHAR ( 50 ) NOT NULL UNIQUE,
	`Passwd`	VARCHAR ( 50 ) NOT NULL,
	`Permissions`	INT ( 5 ) NOT NULL,
	PRIMARY KEY(`UserName`)
);
DROP TABLE IF EXISTS `StudentTable`;
CREATE TABLE IF NOT EXISTS `StudentTable` (
	`preName`	VARCHAR ( 50 ) NOT NULL,
	`surName`	VARCHAR ( 50 ) NOT NULL,
	`address`	VARCHAR ( 200 ),
	`dateOfBirth`	VARCHAR ( 50 ) NOT NULL,
	`comment`	VARCHAR ( 200 ),
	PRIMARY KEY(`preName`,`surName`,`dateOfBirth`)
);
DROP TABLE IF EXISTS `ClassTable`;
CREATE TABLE IF NOT EXISTS `ClassTable` (
	`className`	VARCHAR ( 10 ) NOT NULL UNIQUE,
	`grade`	INT ( 10 ),
	`subject`	VARCHAR ( 50 ),
	PRIMARY KEY(`className`)
);
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
DROP TABLE IF EXISTS `ReturnTable`;
CREATE TABLE IF NOT EXISTS `ReturnTable` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`damage`	VARCHAR ( 50 ),
	`date`	VARCHAR ( 50 ) NOT NULL,
	`price`	VARCHAR ( 50 )
);
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
COMMIT;
