CREATE TABLE `accounts` (
	`id` bigint NOT NULL AUTO_INCREMENT,
	`username` varchar(100) NOT NULL UNIQUE,
	`pwd` varchar(64) NOT NULL,
	`email` varchar(120) NOT NULL,
	PRIMARY KEY (`id`)
);

CREATE TABLE `filelist` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `owner` bigint(20) NOT NULL,
  `fileSize` bigint(20) NOT NULL,
  `file_iv` binary(16) NOT NULL,
  `savedName` varchar(255) NOT NULL,
  `origName` varchar(255) NOT NULL,
  `origFileHash` binary(64) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `FileList_fk0` (`owner`),
  CONSTRAINT `FileList_fk0` FOREIGN KEY (`owner`) REFERENCES `accounts` (`id`)
);