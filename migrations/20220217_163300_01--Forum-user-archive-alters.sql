ALTER TABLE `phpbb_users` ADD `reset_token` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '' AFTER `user_actkey`;
ALTER TABLE `phpbb_users` ADD `reset_token_expiration` int unsigned not null default 0 after `reset_token`;
ALTER TABLE `phpbb_users` DROP `user_email_hash`;
ALTER TABLE `phpbb_users` ADD KEY `user_email` (`user_email`);

ALTER TABLE `phpbb_usersArchive` ADD `reset_token` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '' AFTER `user_actkey`;
ALTER TABLE `phpbb_usersArchive` ADD `reset_token_expiration` int unsigned not null after `reset_token`;
ALTER TABLE `phpbb_usersArchive` DROP `user_email_hash`;
ALTER TABLE `phpbb_usersArchive` ADD KEY `user_email` (`user_email`);