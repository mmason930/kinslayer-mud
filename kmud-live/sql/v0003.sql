CREATE TABLE `guild`(
  `id` int(11) unsigned not null auto_increment,
  `name` varchar(40) not null,
  `created_datetime` datetime not null,
  `created_by_user_id` int(11) unsigned not null,
  `status` tinyint(3) unsigned not null,
  `status_last_modified_datetime` datetime not null,
  `race` tinyint(3) unsigned not null,
  PRIMARY KEY(`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

CREATE TABLE `guildApplication`(
  `id` int(11) unsigned not null auto_increment,
  `guild_name` varchar(40) not null,
  `guild_description` varchar(255) not null,
  `submitted_datetime` datetime not null,
  `submitted_by_user_id` int(11) unsigned not null,
  `submitted_by_user_race` tinyint(3) unsigned not null,
  `status` tinyint(3) unsigned not null,
  `coppers_charged` int(11) unsigned not null,
  `completed_datetime` datetime null default null,
  `reviewer_user_id` int(11) unsigned null default null,
  `denied_reason` varchar(255) null default null,
  `guild_id` int(11) unsigned null default null,
  PRIMARY KEY(`id`),
  KEY `submitted_datetime`(`submitted_datetime`),
  KEY `guild_id`(`guild_id`)
);

CREATE TABLE `userGuild`(
  `id` int(11) unsigned not null auto_increment,
  `user_id` int(11) unsigned not null,
  `guild_id` int(11) unsigned not null,
  `joined_datetime` datetime not null,
  `status` tinyint(3) unsigned not null,
  `status_last_modified_datetime` datetime not null,
  PRIMARY KEY(`id`),
  KEY `user_id`(`user_id`),
  KEY `guild_id`(`guild_id`),
  KEY `user_id_status`(`user_id`, `status`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

CREATE TABLE `guildApplicationSignature`(
  `id` int(11) unsigned not null auto_increment,
  `user_id` int(11) unsigned not null,
  `guild_application_id` int(11) unsigned not null,
  `status` tinyint(3) unsigned not null,
  `created_datetime` datetime not null,
  `status_last_modified_datetime` datetime not null,
  `status_changed_by_user_id` int(11) unsigned not null,
  PRIMARY KEY(`id`),
  KEY `user_id`(`user_id`),
  KEY `guild_application_id`(`guild_application_id`)
) ENGINE=MyISAM CHARSET=utf8 COLLATE=utf8_unicode_ci;


## Statuses:
 # 0) Reviewing
 # 1) Approved
 # 2) Denied
 # 3) Removed
##
CREATE TABLE `guildJoinApplication`(
  `id` int(11) unsigned not null auto_increment,
  `user_id` int(11) unsigned not null,
  `guild_id` int(11) unsigned not null,
  `created_datetime` datetime not null,
  `status` tinyint(3) unsigned not null,
  `status_last_modified_datetime` datetime not null,
  `status_last_modified_by_user_id` int(11) unsigned not null,
  `message_to_guild` text not null,
  `message_from_guild` text null default null,
  PRIMARY KEY(`id`),
  KEY `user_id`(`user_id`),
  KEY `guild_id`(`guild_id`)
) ENGINE=MyISAM charset=utf8 COLLATE=utf8_unicode_ci;

