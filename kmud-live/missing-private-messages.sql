DELETE FROM forums_old.phpbb_users WHERE DATE(FROM_UNIXTIME(user_regdate)) >= '2007-01-10';

DROP TABLE IF EXISTS livemud.tempForumsOldToLivemudUserMap;

CREATE TABLE livemud.tempForumsOldToLivemudUserMap(
  `forums_old_user_id` mediumint(8) unsigned not null,
  `livemud_user_id` mediumint(8) unsigned null,
  UNIQUE KEY `forums_old_user_id`(`forums_old_user_id`),
  KEY `livemud_user_id`(`livemud_user_id`)
) ENGINE=MyISAM;

ALTER TABLE livemud.tempForumsOldToLivemudUserMap DISABLE KEYS;

INSERT INTO livemud.tempForumsOldToLivemudUserMap(
  `forums_old_user_id`
)
SELECT forums_old.phpbb_users.user_id
FROM forums_old.phpbb_users;

ALTER TABLE livemud.tempForumsOldToLivemudUserMap ENABLE KEYS;

#### Create a tempory copy of the user table -- Just User ID and username ####
DROP TABLE IF EXISTS livemud.tempUser;

CREATE TABLE livemud.tempUser(
  `user_id` mediumint(8) unsigned not null,
  `username` varchar(120) not null,
  PRIMARY KEY(`user_id`),
  UNIQUE KEY `username`(`username`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#### Insert all live users into the temp table. ####
INSERT INTO livemud.tempUser
SELECT
  livemud.users.user_id,
  livemud.users.username
FROM
  livemud.users;

#### Insert all archived users into the temp table. ####
INSERT IGNORE INTO livemud.tempUser
SELECT
  livemud.userArchive.user_id,
  livemud.userArchive.username
FROM
  livemud.userArchive;


#### Update the livemud user ID by doing a username match. ####
UPDATE
  livemud.tempForumsOldToLivemudUserMap,
  forums_old.phpbb_users,
  livemud.tempUser
SET
  livemud.tempForumsOldToLivemudUserMap.livemud_user_id = livemud.tempUser.user_id
WHERE LOWER(forums_old.phpbb_users.username) = LOWER(livemud.tempUser.username)
AND forums_old.phpbb_users.user_id = livemud.tempForumsOldToLivemudUserMap.forums_old_user_id;



#### Pull up a list of users for which no match was found. ####
SELECT
  livemud.tempForumsOldToLivemudUserMap.*,
  forums_old.phpbb_users.username
FROM
  livemud.tempForumsOldToLivemudUserMap,
  forums_old.phpbb_users
WHERE forums_old.phpbb_users.user_id = livemud.tempForumsOldToLivemudUserMap.forums_old_user_id
AND livemud.tempForumsOldToLivemudUserMap.livemud_user_id IS NULL;


UPDATE livemud.tempForumsOldToLivemudUserMap, forums_old.phpbb_users, livemud.tempUser SET
livemud.tempForumsOldToLivemudUserMap.livemud_user_id = livemud.tempUser.user_id
WHERE livemud.tempForumsOldToLivemudUserMap.forums_old_user_id = forums_old.phpbb_users.user_id
AND livemud.tempUser.username='Jack'
AND forums_old.phpbb_users.username='Jack Bauer';

UPDATE livemud.tempForumsOldToLivemudUserMap, forums_old.phpbb_users, livemud.tempUser SET
livemud.tempForumsOldToLivemudUserMap.livemud_user_id = livemud.tempUser.user_id
WHERE livemud.tempForumsOldToLivemudUserMap.forums_old_user_id = forums_old.phpbb_users.user_id
AND livemud.tempUser.username='Growler'
AND forums_old.phpbb_users.username='Growla';

UPDATE livemud.tempForumsOldToLivemudUserMap, forums_old.phpbb_users, livemud.tempUser SET
livemud.tempForumsOldToLivemudUserMap.livemud_user_id = livemud.tempUser.user_id
WHERE livemud.tempForumsOldToLivemudUserMap.forums_old_user_id = forums_old.phpbb_users.user_id
AND livemud.tempUser.username='Aveedo'
AND forums_old.phpbb_users.username='1veedo';

UPDATE livemud.tempForumsOldToLivemudUserMap, forums_old.phpbb_users, livemud.tempUser SET
livemud.tempForumsOldToLivemudUserMap.livemud_user_id = livemud.tempUser.user_id
WHERE livemud.tempForumsOldToLivemudUserMap.forums_old_user_id = forums_old.phpbb_users.user_id
AND livemud.tempUser.username='Zangief'
AND forums_old.phpbb_users.username='Farron';

UPDATE livemud.tempForumsOldToLivemudUserMap, forums_old.phpbb_users, livemud.tempUser SET
livemud.tempForumsOldToLivemudUserMap.livemud_user_id = livemud.tempUser.user_id
WHERE livemud.tempForumsOldToLivemudUserMap.forums_old_user_id = forums_old.phpbb_users.user_id
AND livemud.tempUser.username='Jack'
AND forums_old.phpbb_users.username='JackTheGreat';

UPDATE livemud.tempForumsOldToLivemudUserMap, forums_old.phpbb_users, livemud.tempUser SET
livemud.tempForumsOldToLivemudUserMap.livemud_user_id = livemud.tempUser.user_id
WHERE livemud.tempForumsOldToLivemudUserMap.forums_old_user_id = forums_old.phpbb_users.user_id
AND livemud.tempUser.username='Doman'
AND forums_old.phpbb_users.username='Domon';



| privmsgs_text_id    | mediumint(8) unsigned | NO   | PRI | 0       |       |
| privmsgs_bbcode_uid | varchar(10)           | NO   |     | 0       |       |
| privmsgs_text       | text                  | YES  |     | NULL    |       |


| privmsgs_id             | mediumint(8) unsigned | NO   | PRI | NULL    | auto_increment |
| privmsgs_type           | tinyint(4)            | NO   |     | 0       |                |
| privmsgs_subject        | varchar(255)          | NO   |     | 0       |                |
| privmsgs_from_userid    | mediumint(8)          | NO   | MUL | 0       |                |
| privmsgs_to_userid      | mediumint(8)          | NO   | MUL | 0       |                |
| privmsgs_date           | int(11)               | NO   |     | 0       |                |
| privmsgs_ip             | varchar(8)            | NO   |     |         |                |
| privmsgs_enable_bbcode  | tinyint(1)            | NO   |     | 1       |                |
| privmsgs_enable_html    | tinyint(1)            | NO   |     | 0       |                |
| privmsgs_enable_smilies | tinyint(1)            | NO   |     | 1       |                |
| privmsgs_attach_sig     | tinyint(1)            | NO   |     | 1       |                |

mysql> describe phpbb_privmsgs;
+---------------------+-----------------------+------+-----+---------+----------------+
| Field               | Type                  | Null | Key | Default | Extra          |
+---------------------+-----------------------+------+-----+---------+----------------+
| msg_id              | mediumint(8) unsigned | NO   | PRI | NULL    | auto_increment |
| root_level          | mediumint(8) unsigned | NO   | MUL | 0       |                |
| author_id           | mediumint(8) unsigned | NO   | MUL | 0       |                |
| icon_id             | mediumint(8) unsigned | NO   |     | 0       |                |
| author_ip           | varchar(40)           | NO   | MUL |         |                |
| message_time        | int(11) unsigned      | NO   | MUL | 0       |                |
| enable_bbcode       | tinyint(1) unsigned   | NO   |     | 1       |                |
| enable_smilies      | tinyint(1) unsigned   | NO   |     | 1       |                |
| enable_magic_url    | tinyint(1) unsigned   | NO   |     | 1       |                |
| enable_sig          | tinyint(1) unsigned   | NO   |     | 1       |                |
| message_subject     | varchar(255)          | NO   |     |         |                |
| message_text        | mediumtext            | NO   |     | NULL    |                |
| message_edit_reason | varchar(255)          | NO   |     |         |                |
| message_edit_user   | mediumint(8) unsigned | NO   |     | 0       |                |
| message_attachment  | tinyint(1) unsigned   | NO   |     | 0       |                |
| bbcode_bitfield     | varchar(255)          | NO   |     |         |                |
| bbcode_uid          | varchar(8)            | NO   |     |         |                |
| message_edit_time   | int(11) unsigned      | NO   |     | 0       |                |
| message_edit_count  | smallint(4) unsigned  | NO   |     | 0       |                |
| to_address          | text                  | NO   |     | NULL    |                |
| bcc_address         | text                  | NO   |     | NULL    |                |
| message_reported    | tinyint(1) unsigned   | NO   |     | 0       |                |
+---------------------+-----------------------+------+-----+---------+----------------+
22 rows in set (0.00 sec)
