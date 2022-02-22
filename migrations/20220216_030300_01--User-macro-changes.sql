ALTER TABLE `userMacro`
DROP KEY `user_id_key_code`,
ADD `location` tinyint(3) unsigned null default null,
ADD UNIQUE KEY `user_id_key_code_location`(`user_id`, `key_code`, `location`);

ALTER TABLE `userMacro`
ADD `code` varchar(64) not null default '';