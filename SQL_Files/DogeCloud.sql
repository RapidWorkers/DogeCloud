CREATE TABLE `accounts` (
	`id` bigint NOT NULL AUTO_INCREMENT,
	`username` varchar(100) NOT NULL UNIQUE,
	`pwd` varchar(64) NOT NULL,
	`email` varchar(120) NOT NULL,
	PRIMARY KEY (`id`)
);

CREATE TABLE `FileList` (
	`id` bigint NOT NULL AUTO_INCREMENT,
	`owner` bigint NOT NULL,
	`fileSize` bigint NOT NULL,
	`file_iv` BINARY(16) NOT NULL,
	`directory` varchar(255) NOT NULL,
	`savedName` varchar(255) NOT NULL,
	`origName` varchar(255) NOT NULL,
	`origFileHash` BINARY(64) NOT NULL,
	PRIMARY KEY (`id`)
);

CREATE TABLE `FileDirectory` (
	`id` bigint NOT NULL AUTO_INCREMENT,
	`dirName` varchar(255) NOT NULL,
	`owner` bigint NOT NULL,
	PRIMARY KEY (`id`)
);

ALTER TABLE `FileList` ADD CONSTRAINT `FileList_fk0` FOREIGN KEY (`owner`) REFERENCES `accounts`(`id`);
ALTER TABLE `FileDirectory` ADD CONSTRAINT `FileDirectory_fk0` FOREIGN KEY (`owner`) REFERENCES `accounts`(`id`);