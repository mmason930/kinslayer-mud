DROP TABLE IF EXISTS `scriptTrigger`;

CREATE TABLE `scriptTrigger`(
  `id` int(11) unsigned not null,
  `script_id` int(11) unsigned not null,
  `name` varchar(50) not null,
  `type` char(1) not null,
  `narg` smallint(5) unsigned not null,
  `args` varchar(50) not null,
  `trigger_flags` bigint(20) unsigned not null,
  `allowed_flags` int(10) unsigned not null,
  `option_flags` int(11) unsigned not null,
  PRIMARY KEY(`id`),
  KEY `script_id`(`script_id`)
) ENGINE=MyISAM;

INSERT INTO scriptTrigger
SELECT
  vnum,
  vnum,
  name,
  type,
  narg,
  args,
  trigger_flags,
  allowed_flags,
  option_flags
FROM js_scripts
WHERE vnum > 0;

DROP TABLE IF EXISTS `script`;

CREATE TABLE script(
  `id` int(11) unsigned not null auto_increment,
  `method_name` varchar(255) not null,
  `created_by_user_id` int(11) unsigned not null,
  `created_datetime` datetime not null,
  `last_modified_by_user_id` int(11) unsigned not null,
  `last_modified_datetime` datetime not null,
  PRIMARY KEY(`id`)
) ENGINE=MyISAM;

INSERT INTO script
SELECT
  vnum,
  CONCAT("script", vnum),
  1,
  NOW(),
  1,
  NOW()
FROM js_scripts
WHERE vnum >= 0;

CREATE TABLE scriptImportQueue(
  `id` bigint(20) unsigned not null auto_increment,
  `file_path` varchar(255) not null,
  `queued_datetime` datetime not null,
  `operation` tinyint(3) unsigned not null,
  PRIMARY KEY(`id`)
) ENGINE=MyISAM;

ALTER TABLE mob_protos ADD `moves_per_tic` smallint(5) not null DEFAULT -1;
