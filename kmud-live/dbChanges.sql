CREATE TABLE  `livemud`.`userDeletion` (
`id` INT NOT NULL AUTO_INCREMENT PRIMARY KEY ,
`user_id` MEDIUMINT NOT NULL ,
`created_datetime` DATETIME NOT NULL
) ENGINE = MYISAM;

CREATE TABLE  `livemud`.`userRestore` (
`id` INT NOT NULL AUTO_INCREMENT PRIMARY KEY ,
`user_id` MEDIUMINT NOT NULL ,
`created_datetime` DATETIME NOT NULL
) ENGINE = MYISAM ;

CREATE TABLE `livemud`.`userLogout` (
`id` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
`user_id` INT NOT NULL,
`created_datetime` DATETIME NOT NULL,
`logout_type` TINYINT NOT NULL,
`room_id` MEDIUMINT NULL DEFAULT NULL
) ENGINE=MYISAM;

CREATE TABLE `gameSession` (
`id` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
`start_datetime` DATETIME NOT NULL,
`end_datetime` DATETIME NULL DEFAULT NULL
);

CREATE TABLE userSession (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `user_id` int(11) NOT NULL,
  `session_key` varchar(40) NOT NULL,
  `created_datetime` datetime NOT NULL,
  PRIMARY KEY(`id`),
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM;

CREATE TABLE clan (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(40) NOT NULL,
  `is_secret` tinyint(3) unsigned NOT NULL,
  `warrant_id` tinyint(3) unsigned NULL,
  `hidden_level` tinyint unsigned NULL,
  `bonus_skill_id` smallint(7) unsigned NULL,
  `bonus_skill_modifier` tinyint(3) NULL,
  `bonus_skill_rank` tinyint unsigned NULL,
  PRIMARY KEY(`id`)
) ENGINE=MyISAM;

CREATE TABLE clanRank (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `clan_id` int(11) unsigned NOT NULL,
  `rank_number` tinyint(11) unsigned NOT NULL,
  `name` varchar(40) NOT NULL,
  PRIMARY KEY(`id`),
  KEY `clan_id`(`clan_id`)
) ENGINE=MyISAM;

DROP TABLE
`weave_affects`,
`weave_objects`,
`weave_prototypes`;

CREATE TABLE `weaveAttribute` (
 `name` varchar(40) NOT NULL,
 `attribute_name` varchar(40) NOT NULL,
 `attribute_value` varchar(512) NOT NULL,
 UNIQUE KEY `name` (`name`,`attribute_name`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE skillMessage(
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `type` tinyint(3) unsigned NOT NULL,
  `die_attacker_text` varchar(255) NULL,
  `die_victim_text` varchar(255) NULL,
  `die_room_text` varchar(255) NULL,
  
  `miss_attacker_text` varchar(255) NULL,
  `miss_victim_text` varchar(255) NULL,
  `miss_room_text` varchar(255) NULL,

  `hit_attacker_text` varchar(255) NULL,
  `hit_victim_text` varchar(255) NULL,
  `hit_room_text` varchar(255) NULL,

  `god_attacker_text` varchar(255) NULL,
  `god_victim_text` varchar(255) NULL,
  `god_room_text` varchar(255) NULL,
  PRIMARY KEY(`id`),
  KEY `type`(`type`)
) ENGINE=MyISAM;

CREATE TABLE `config` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(40) NOT NULL,
  `value` varchar(6000) NOT NULL,
  PRIMARY KEY(`id`)
) ENGINE=MyISAM;

CREATE TABLE `statGroup` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `race` tinyint(3) unsigned NOT NULL,
  `chclass` tinyint(3) unsigned NOT NULL,
  `sex` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY(`id`)
) ENGINE=MyISAM;

CREATE TABLE `statGroupStat` (
  `stat_group_id` int(11) unsigned NOT NULL,
  `stat_type` tinyint(3) unsigned NOT NULL,
  `stat_roll` tinyint(3) unsigned NOT NULL,
  `probability` tinyint(3) unsigned NOT NULL,
  KEY `stat_group_id`(`stat_group_id`)
) ENGINE=MyISAM;

CREATE TABLE `shop` (
  `id` int(11) unsigned NOT NULL,
  `profit_buy` float NOT NULL,
  `profit_sell` float NOT NULL,
  `open_time_1` tinyint(3) NOT NULL,
  `close_time_1` tinyint(3) NOT NULL,
  `open_time_2` tinyint(3) NOT NULL,
  `close_time_2` tinyint(3) NOT NULL,
  `no_such_item_1` varchar(255) NULL DEFAULT NULL,
  `no_such_item_2` varchar(255) NULL DEFAULT NULL,
  `do_not_buy` varchar(255) NULL DEFAULT NULL,
  `missing_cash_1` varchar(255) NULL DEFAULT NULL,
  `missing_cash_2` varchar(255) NULL DEFAULT NULL,
  `buy` varchar(255) NULL DEFAULT NULL,
  `sell` varchar(255) NULL DEFAULT NULL,
  `bitvector` int(11) NOT NULL,
  `keeper_vnum` mediumint NOT NULL,
  `will_trade_with` int(11) NOT NULL,
  PRIMARY KEY(`id`)
) ENGINE=MyISAM;

CREATE TABLE `shopProducing` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `shop_id` int(11) unsigned NOT NULL,
  `object_vnum` int(11) unsigned NOT NULL,
  PRIMARY KEY(`id`),
  KEY `shop_id`(`shop_id`),
  KEY `object_vnum`(`object_vnum`)
) ENGINE=MyISAM;

CREATE TABLE `shopBuyType` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `shop_id` int(11) unsigned NOT NULL,
  `object_type` tinyint(3) unsigned NOT NULL,
  `keywords` varchar(255) NOT NULL,
  PRIMARY KEY(`id`),
  KEY `shop_id`(`shop_id`)
) ENGINE=MyISAM;

CREATE TABLE `shopRoom` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `shop_id` int(11) unsigned NOT NULL,
  `room_id` int(11) unsigned NOT NULL,
  PRIMARY KEY(`id`),
  KEY `shop_id`(`shop_id`)
) ENGINE=MyISAM;


### May 28 2012 ###
CREATE TABLE clanQuestPointTransaction(
  `id` int(11) unsigned not null auto_increment,
  `clan_id` smallint(6) unsigned not null,
  `user_id` mediumint(9) unsigned not null,
  `amount` smallint(6) not null,
  `created_datetime` datetime not null,
  `issued_by_user_type` tinyint(3) unsigned not null,
  `issued_by_user_id` tinyint(3) unsigned not null,
  `reason` varchar(120) not null,
  PRIMARY KEY(`id`),
  KEY `clan_id`(`clan_id`),
  KEY `user_id`(`user_id`),
  KEY `issued_by_user_id`(`issued_by_user_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
