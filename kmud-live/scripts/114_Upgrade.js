var script114 = function(self, actor, here, args, extra)
{
	_block;
actor.send("Upgrade command has been disabled.");
return;
if( actor.name != "Galnor" ) {
	actor.send("You are not allowed to do this.");
	return;
}
var queue = [];
var f;
function encode64(input) {
	var keyStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
	var output = "";
	var chr1, chr2, chr3;
	var enc1, enc2, enc3, enc4;
	var i = 0;
	while (i < input.length) {
		chr1 = input.charCodeAt(i++);
		chr2 = input.charCodeAt(i++);
		chr3 = input.charCodeAt(i++);
		enc1 = chr1 >> 2;
		enc2 = ((chr1 & 3) << 4) | (chr2 >> 4);
		enc3 = ((chr2 & 15) << 2) | (chr3 >> 6);
		enc4 = chr3 & 63;
		if (isNaN(chr2)) {
			enc3 = enc4 = 64;
		} else if (isNaN(chr3)) {
			enc4 = 64;
		}
		output += (keyStr.charAt(enc1) + keyStr.charAt(enc2) + keyStr.charAt(enc3) + keyStr.charAt(enc4));
   }
  
	return output;
}
function decode64(input) {
	var keyStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
//	var output = new StringMaker();
	var output = "";
	var chr1, chr2, chr3;
	var enc1, enc2, enc3, enc4;
	var i = 0;
	// remove all characters that are not A-Z, a-z, 0-9, +, /, or =
	input = input.replace(/[^A-Za-z0-9\+\/\=]/g, "");
	while (i < input.length) {
		enc1 = keyStr.indexOf(input.charAt(i++));
		enc2 = keyStr.indexOf(input.charAt(i++));
		enc3 = keyStr.indexOf(input.charAt(i++));
		enc4 = keyStr.indexOf(input.charAt(i++));
		chr1 = (enc1 << 2) | (enc2 >> 4);
		chr2 = ((enc2 & 15) << 4) | (enc3 >> 2);
		chr3 = ((enc3 & 3) << 6) | enc4;
		output += String.fromCharCode(chr1);
		if (enc3 != 64) {
			output = String.fromCharCode(chr2)
		}
		if (enc4 != 64) {
			output += String.fromCharCode(chr3);
		}
	}
	return output;
}
function BitField() {
	this.data = encode64('');
	
	this.set = function( n ) {
		
		var byte = n >> 3;
		var bit = 7 - (n & 7);
		
		if( this.data.length >= byte + 1 ) {
			var front = this.data.substr(0,byte);
			var back = this.data.substr(byte+1);
			
			this.data = front + String.fromCharCode(this.data.charCodeAt(byte) | (1 << bit)) + back;
		}
		else {
		
			this.data += strPadding("","\0",byte-this.data.length,"right");
			this.data += String.fromCharCode(1<<bit);
		}
//			self.data = self.data + str_repeat("\0", byte - len(self.data))
//			self.data = self.data + chr(1 << bit)
	}
	this.get_base64 = function() {
		return encode64(this.data);
	}
	
	return this;
}
/***
class bitfield:
	def __init__(self, base=''):
		self.data = base64.encodestring(base)
		
	def set( self, n ):
		byte = n >> 3
		bit = 7 - (n & 7)
		if( len(self.data) >= byte + 1):
			self.data = AlterString( self.data, byte, chr(ord(self.data[byte]) | (1 << bit)) )
		else:
			self.data = self.data + str_repeat("\0", byte - len(self.data))
			self.data = self.data + chr(1 << bit)
	def get_base64(self):
		print self.data
		return base64.encodestring(self.data)
		
***/
		
function addToQueue(f,s) {
	var queueObject = new Object();
	
	queueObject.handler = f;
	queueObject.message = s;
	
	queue.push( queueObject );
}
f = function() {
	sqlQuery("DROP TABLE IF EXISTS tempUsers");
}
addToQueue(f,"Dropping tempUsers...");
f = function() {
	sqlQuery("CREATE TABLE tempUsers LIKE users");
}
addToQueue(f,"Creating tempUsers...");
f = function() {
	sqlQuery(
			"ALTER TABLE tempUsers "
		+	"ADD `user_type` tinyint(2) NOT NULL default '0',"
		+	"ADD `group_id` mediumint(8) unsigned NOT NULL default '3',"
		+	"ADD `user_permissions` mediumtext collate utf8_bin NOT NULL,"
		+	"ADD `user_perm_from` mediumint(8) unsigned NOT NULL default '0',"
		+	"ADD `user_ip` varchar(40) collate utf8_bin NOT NULL default '',"
	//	+	"ADD `user_regdate` int(11) unsigned NOT NULL default '0',"
	//	+	"ADD `username` varchar(255) collate utf8_bin NOT NULL default '',"
		+	"ADD `username_clean` varchar(255) collate utf8_bin NOT NULL default '',"
		+	"ADD `user_passchg` int(11) unsigned NOT NULL default '0',"
		+	"ADD `user_pass_convert` tinyint(1) unsigned NOT NULL default '0',"
	//	+	"ADD `user_email` varchar(100) collate utf8_bin NOT NULL default '',"
		+	"ADD `user_email_hash` bigint(20) NOT NULL default '0',"
		+	"ADD `user_birthday` varchar(10) collate utf8_bin NOT NULL default '',"
	//	+	"ADD `user_lastvisit` int(11) unsigned NOT NULL default '0',"
		+	"ADD `user_lastmark` int(11) unsigned NOT NULL default '0',"
		+	"ADD `user_lastpost_time` int(11) unsigned NOT NULL default '0',"
		+	"ADD `user_lastpage` varchar(200) collate utf8_bin NOT NULL default '',"
		+	"ADD `user_last_confirm_key` varchar(10) collate utf8_bin NOT NULL default '',"
		+	"ADD `user_last_search` int(11) unsigned NOT NULL default '0',"
		+	"ADD `user_warnings` tinyint(4) NOT NULL default '0',"
		+	"ADD `user_last_warning` int(11) unsigned NOT NULL default '0',"
		+	"ADD `user_login_attempts` tinyint(4) NOT NULL default '0',"
		+	"ADD `user_inactive_reason` tinyint(2) NOT NULL default '0',"
		+	"ADD `user_inactive_time` int(11) unsigned NOT NULL default '0',"
	//	+	"ADD `user_posts` mediumint(8) unsigned NOT NULL default '0'"
	//	+	"ADD `user_lang` varchar(30) collate utf8_bin NOT NULL default '',"
	//	+	"ADD `user_timezone` decimal(5,2) NOT NULL default '0.00',"
		+	"ADD `user_dst` tinyint(1) unsigned NOT NULL default '0',"
	//	+	"ADD `user_dateformat` varchar(30) collate utf8_bin NOT NULL default 'd M Y H:i',"
	//	+	"ADD `user_style` mediumint(8) unsigned NOT NULL default '0',"
	//	+	"ADD `user_rank` mediumint(8) unsigned NOT NULL default '0',"
		+	"ADD `user_colour` varchar(6) collate utf8_bin NOT NULL default '',"
	//	+	"ADD `user_new_privmsg` int(4) NOT NULL default '0',"
	//	+	"ADD `user_unread_privmsg` int(4) NOT NULL default '0',"
	//	+	"ADD `user_last_privmsg` int(11) unsigned NOT NULL default '0',"
		+	"ADD `user_message_rules` tinyint(1) unsigned NOT NULL default '0',"
		+	"ADD `user_full_folder` int(11) NOT NULL default '-3',"
	//	+	"ADD `user_emailtime` int(11) unsigned NOT NULL default '0',"
		+	"ADD `user_topic_show_days` smallint(4) unsigned NOT NULL default '0',"
		+	"ADD `user_topic_sortby_type` varchar(1) collate utf8_bin NOT NULL default 't',"
		+	"ADD `user_topic_sortby_dir` varchar(1) collate utf8_bin NOT NULL default 'd',"
		+	"ADD `user_post_show_days` smallint(4) unsigned NOT NULL default '0',"
		+	"ADD `user_post_sortby_type` varchar(1) collate utf8_bin NOT NULL default 't',"
		+	"ADD `user_post_sortby_dir` varchar(1) collate utf8_bin NOT NULL default 'a',"
	//	+	"ADD `user_notify` tinyint(1) unsigned NOT NULL default '0',"
	//	+	"ADD `user_notify_pm` tinyint(1) unsigned NOT NULL default '1',"
		+	"ADD `user_notify_type` tinyint(4) NOT NULL default '0',"
	//	+	"ADD `user_allow_pm` tinyint(1) unsigned NOT NULL default '1',"
	//	+	"ADD `user_allow_viewonline` tinyint(1) unsigned NOT NULL default '1',"
		+	"ADD `user_allow_viewemail` tinyint(1) unsigned NOT NULL default '1',"
		+	"ADD `user_allow_massemail` tinyint(1) unsigned NOT NULL default '1',"
	//	+	"ADD `user_options` int(11) unsigned NOT NULL default '230271',"
	//	+	"ADD `user_avatar` varchar(255) collate utf8_bin NOT NULL default '',"
	//	+	"ADD `user_avatar_type` tinyint(2) NOT NULL default '0',"
		+	"ADD `user_avatar_width` smallint(4) unsigned NOT NULL default '0',"
		+	"ADD `user_avatar_height` smallint(4) unsigned NOT NULL default '0',"
	//	+	"ADD `user_sig` mediumtext collate utf8_bin NOT NULL,"
	//	+	"ADD `user_sig_bbcode_uid` varchar(8) collate utf8_bin NOT NULL default '',"
		+	"ADD `user_sig_bbcode_bitfield` varchar(255) collate utf8_bin NOT NULL default '',"
	//	+	"ADD `user_from` varchar(100) collate utf8_bin NOT NULL default '',"
	//	+	"ADD `user_icq` varchar(15) collate utf8_bin NOT NULL default '',"
	//	+	"ADD `user_aim` varchar(255) collate utf8_bin NOT NULL default '',"
	//	+	"ADD `user_yim` varchar(255) collate utf8_bin NOT NULL default '',"
	//	+	"ADD `user_msnm` varchar(255) collate utf8_bin NOT NULL default '',"
		+	"ADD `user_jabber` varchar(255) collate utf8_bin NOT NULL default '',"
	//	+	"ADD `user_website` varchar(200) collate utf8_bin NOT NULL default '',"
	//	+	"ADD `user_occ` text collate utf8_bin NOT NULL,"
	//	+	"ADD `user_interests` text collate utf8_bin NOT NULL,"
	//	+	"ADD `user_actkey` varchar(32) collate utf8_bin NOT NULL default '',"
	//	+	"ADD `user_newpasswd` varchar(40) collate utf8_bin NOT NULL default '',"
		+	"ADD `user_form_salt` varchar(32) collate utf8_bin NOT NULL default '',"
		+	"ADD `user_new` tinyint(1) unsigned NOT NULL default '1',"
		+	"ADD `user_reminded` tinyint(4) NOT NULL default '0',"
		+	"ADD `user_reminded_time` int(11) unsigned NOT NULL default '0'"
	);
}
addToQueue(f,"Altering tempUsers schema...");
f = function() {
	var sql =
			   "INSERT INTO tempUsers SELECT"
		+	   " `username`,"
		+	   " `user_password`,"
		+	   " `user_id`,"
		+	   " `poofin`,"
		+	   " `poofout`,"
		+	   " `level`,"
		+	   " `maxhit`,"
		+	   " `hit`,"
		+	   " `maxmove`,"
		+	   " `move`,"
		+	   " `mana`,"
		+	   " `maxmana`,"
		+	   " `shadow`,"
		+	   " `maxshadow`,"
		+	   " `race`,"
		+	   " `chclass`,"
		+	   " `weight`,"
		+	   " `height`,"
		+	   " `sex`,"
		+	   " `weave`,"
		+	   " `invis`,"
		+	   " `taint`,"
		+	   " `death_wait`,"
		+	   " `warnings`,"
		+	   " `master_weapon`,"
		+	   " `strain`,"
		+	   " `slew`,"
		+	   " `whois_extra`,"
		+	   " `last_logon`,"
		+	   " `last_logout`,"
		+	   " `gold`,"
		+	   " `bank_gold`,"
		+	   " `birth`,"
		+	   " `str`,"
		+	   " `dex`,"
		+	   " `intel`,"
		+	   " `wis`,"
		+	   " `con`,"
		+	   " `luck`,"
		+	   " `exp`,"
		+	   " `played`,"
		+	   " `restat`,"
		+	   " `pk_deaths`,"
		+	   " `mob_deaths`,"
		+	   " `mount`,"
		+	   " `loadroom`,"
		+	   " `pracs`,"
		+	   " `spell_pracs`,"
		+	   " `trade_pracs`,"
		+	   " `freeze`,"
		+	   " `bad_passwords`,"
		+	   " `hunger`,"
		+	   " `thirst`,"
		+	   " `drunk`,"
		+	   " `reset`,"
		+	   " `prf`,"
		+	   " `plr`,"
		+	   " `newb`,"
		+	   " `warrants`,"
		+	   " `title`,"
		+	   " `description`,"
		+	   " `user_interests`,"
		+	   " `user_occ`,"
		+	   " `user_msnm`,"
		+	   " `user_yim`,"
		+	   " `user_aim`,"
		+	   " `user_sig_bbcode_uid`,"
		+	   " `user_sig`,"
		+	   " `user_from`,"
		+	   " `user_website`,"
		+	   " `user_icq`,"
		+	   " `user_email`,"
		+	   " `user_avatar_type`,"
		+	   " `user_avatar`,"
		+	   " `user_rank`,"
		+	   " `user_popup_pm`,"
		+	   " `user_notify_pm`,"
		+	   " `user_notify`,"
		+	   " `user_allow_viewonline`,"
		+	   " `user_allow_pm`,"
		+	   " `user_allowavatar`,"
		+	   " `user_allowsmile`,"
		+	   " `user_allowbbcode`,"
		+	   " `user_allowhtml`,"
		+	   " `user_attachsig`,"
		+	   " `user_viewemail`,"
		+	   " `user_emailtime`,"
		+	   " `user_last_login_try`,"
		+	   " `user_login_tries`,"
		+	   " `user_last_privmsg`,"
		+	   " `user_unread_privmsg`,"
		+	   " `user_new_privmsg`,"
		+	   " `user_lang`,"
		+	   " '2'," //user_style
		+	   " `user_timezone`,"
		+	   " `user_posts`,"
		+	   " `user_level`,"
		+	   " `user_regdate`,"
		+	   " `user_lastvisit`,"
		+	   " `user_session_page`,"
		+	   " `user_sessiontime`,"
		+	   " `user_active`,"
		+	   " `user_actkey`,"
		+	   " `user_newpasswd`,"
		+	   " `user_session_time`,"
		+	   " `user_dateformat`,"
		+	   " `pks`,"
		+	   " `pw_updated`,"
		+	   " `user_show_quickreply`,"
		+	   " `user_quickreply_mode`,"
		+	   " `user_open_quickreply`,"
		+	   " `user_real_name`,"
		+	   " `user_newpassword`,"
		+	   " `user_newpass_time`,"
		+	   " `user_options`,"
		+	   " `user_touched`,"
		+	   " `user_token`,"
		+	   " `user_email_authenticated`,"
		+	   " `user_email_token`,"
		+	   " `user_email_token_expires`,"
		+	   " `user_registration`,"
		+	   " `user_editcount`,"
		+	   " `account_id`,"
		+	   " 3,"
		+	   " 1,"
		+	   " '',"
		+	   " 0,"
		+	   " '',"
		+	   "`username`,"
		+	   " 0,"
		+	   " 0,"
		+	   " 0,"//TODO: <Email Hash> : ???
		+	   " '',"
		+	   " 0,"
		+	   " 0,"
		+	   " '',"
		+	   " '',"
		+	   " 0,"
		+	   " 0,"
		+	   " 0,"
		+	   " 0,"
		+	   " 0,"
		+	   " 0,"
		+	   " 0,"
		+	   " 'AA0000',"
		+	   " 0,"
		+	   " -3,"
		+	   " 0,"
		+	   " 't',"
		+	   " 'd',"
		+	   " 0,"
		+	   " 't',"
		+	   " 'a',"
		+	   " 0,"
		+	   " 1,"
		+	   " 1,"
		+	   " 100,"//Avatar width
		+	   " 100,"//Avatar height
		+	   " '',"//User sig bbcode bitfield
		+	   " '',"
		+	   " '',"//User form salt?
		+	   " 0,"
		+	   " 0,"
		+	   " 0"
		+	   " FROM users;";
		
		sqlQuery( sql );
	}
	addToQueue(f,"Copying users into tempUsers...");
	
	f = function() {
		sql = "UPDATE tempUsers SET username_clean=LOWER(username_clean)";
		sqlQuery( sql );
	}
	addToQueue(f,"Updating username_clean field...");
	f = function() {
		sql = "DELETE FROM phpbb_forums";
		sqlQuery( sql );
	}
	addToQueue(f,"Deleting posts in new post table...");
	f = function() {
		sql = "INSERT INTO `phpbb_forums` (`forum_id`, `parent_id`, `left_id`, `right_id`, `forum_parents`, `forum_name`, `forum_desc`, `forum_desc_bitfield`, `forum_desc_options`, `forum_desc_uid`, `forum_link`, `forum_password`, `forum_style`, `forum_image`, `forum_rules`, `forum_rules_link`, `forum_rules_bitfield`, `forum_rules_options`, `forum_rules_uid`, `forum_topics_per_page`, `forum_type`, `forum_status`, `forum_posts`, `forum_topics`, `forum_topics_real`, `forum_last_post_id`, `forum_last_poster_id`, `forum_last_post_subject`, `forum_last_post_time`, `forum_last_poster_name`, `forum_last_poster_colour`, `forum_flags`, `forum_options`, `display_subforum_list`, `display_on_index`, `enable_indexing`, `enable_icons`, `enable_prune`, `prune_next`, `prune_days`, `prune_viewed`, `prune_freq`) VALUES(1, 0, 1, 12, '', 'Announcements', '', '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 0, 0, 1, 1, 1, 1, 2, '', 1278354759, 'mistaki', 'AA0000', 32, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0),(2, 1, 2, 3, '', 'Immortal Announcements', 0x496d706f7274616e7420616e6e6f756e63656d656e74732066726f6d20746865204b696e736c617965724d55442053746166662e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 2, 1, 1, 2, 1, 'Re: Welcome to phpBB3', 1278922601, 'Galnor', 'AA0000', 32, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0),(3, 1, 4, 5, '', 'Game &amp; Forum Rules', 0x41206c697374206f662072756c657320666f7220746865204d554420616e642069747320666f72756d732e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(4, 1, 6, 7, '', 'Far Allein', 0x466f72207468652074616c65732c2074726561746965732c207374727567676c65732c20616e6420626174746c6573206f66207468652048756d616e20526163652e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(5, 1, 8, 9, '', 'Far Shar', 0x466f72207468652068617070656e696e6773206f6620746865204461726b204f6e6527732061726d69657320616e64207468656972206c75737420746f20636f6e717565722e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(6, 1, 10, 11, '', 'Auction House', 0x43757272656e74206f66666572696e6773206f66207468652061756374696f6e20686f75736573207468726f7567686f757420746865206c616e64732e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(7, 9, 14, 15, '', 'General', 0x4272696e67207570206e65772069646561732026616d703b2064697363757373206578697374696e672066656174757265732e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(8, 9, 16, 17, '', 'Getting Started', 0x4120706c616365207768657265206e657720706c61796572732063616e206765742068656c7020616e642068617665207468656972207175657374696f6e277320616e7377657265642e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(9, 0, 13, 30, '', 'For All Players', '', '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 0, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(11, 9, 18, 19, '', 'Ask The Imms', 0x4120466f72756d20746f2061736b2074686520496d6d732e2042652070617469656e742c20627574207765276c6c2074727920746f20616e7377657220617320736f6f6e20617320706f737369626c652e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(12, 9, 20, 21, '', 'Chill &amp; Chat', 0x4120666f72756d20666f7220616e797468696e6720616e642065766572797468696e672e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(13, 9, 22, 23, '', 'The Art of Battle', 0x506f7374206772656174206c6f67732c206570696320626174746c6573206f72206469736375737320746163746963732e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(14, 9, 24, 25, '', 'Roleplay Library', 0x4120706c61636520746f20706f737420726f6c65706c6179206c6f677320616e64206469736375737320726f6c65706c61792e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(15, 9, 26, 27, '', 'Bugs and Loopholes', 0x546f20706f7374206275677320616e64206c6f6f70686f6c65732e20496620796f752066696e64206f6e652c20706f73742069742e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(16, 9, 28, 29, '', 'Humor', 0x466f722066756e6e792071756f7465732c206c6f67732c20616e64206c696e6b732e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(17, 0, 31, 52, '', 'Clans of the Light', '', '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 0, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(18, 0, 53, 74, '', 'Clans of the Dark', '', '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 0, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(19, 0, 75, 98, '', 'Immortal Corner', '', '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 0, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(20, 17, 32, 33, '', 'Borderguards', 0x466f722074686520736f6c6469657273206f6620746865206e6f72746820616e6420646566656e64657273206f6620536869656e61722e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(21, 17, 34, 35, '', 'Queen''s Guards', 0x466f722074686520646566656e64657273206f6620416e646f722e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(22, 17, 36, 37, '', 'Valon', 0x466f722074686520646566656e64657273206f66205461722056616c6f6e2e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(23, 17, 38, 39, '', 'Murandian Cavalry', 0x466f722074686520646566656e64657273206f66204d7572616e64792e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(24, 17, 40, 41, '', 'The White Tower', 0x466f7220746865204e6f76696365732c2041636365707465642c20616e6420416573205365646169206f662074686520576869746520546f7765722e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(25, 17, 42, 43, '', 'Aes Sedai', 0x466f722074686520416573205365646169206f662074686520576869746520546f776572, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(26, 17, 44, 45, '', 'Gaidin', 0x466f72207468652076616c69616e7420646566656e64657273206f662074686520666c616d652e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(27, 17, 46, 47, '', 'Blademasters', 0x466f7220746865206d6f737420656c6974652073776f7264736d656e206f6620746865204c696768742e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(28, 17, 48, 49, '', 'Merchants Guild', 0x466f7220746865204d65726368616e7473206f66204d61726b65742053686172616e2e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(29, 17, 50, 51, '', 'Wolfbrothers', 0x466f72207468652062726f74686572686f6f64206f662074686520776f6c7665732e204120706c61636520746f20756e6974652e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(30, 18, 54, 55, '', 'Ghob''hlin', 0x466f7220746865206c61726765737420616e64207374726f6e67657374206f6620746865204772656174204c6f72642773206d696e696f6e732e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(31, 18, 56, 57, '', 'Ko''bal', 0x466f722074686520656c69746520617373617373696e73206f662074686520536861646f772e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(32, 18, 58, 59, '', 'Dhjin''nen', 0x466f72207468652044686a696e276e656e2e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(33, 18, 60, 61, '', 'Dreadguards', 0x5468652062657374206f66207468652044726561646775617264732c206d656e20616e6420776f6d656e20776974682074686520726172652067696674206f66206368616e6e656c696e672077686f20706c656467656420616c6c656769616e636520746f2074686520536861646f772c20636f6e76656e6520686572652e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(34, 18, 62, 63, '', 'Myrddraal', 0x466f722074686f73652077686f2068617665206265656e20626c696e64656420627920746865204461726b2e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(35, 18, 64, 65, '', 'Dha''vol', 0x466f7220746865204772656174204c6f726427732066696e6573742073636f7574732e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(36, 18, 66, 67, '', 'Al''ghol', 0x466f722074686520416c2767686f6c2e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(37, 18, 68, 69, '', 'Shadow Council', 0x466f7220746865206c656164657273206f6620746865204461726b204f6e652773206d696e696f6e732e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(38, 18, 70, 71, '', 'Kno''mon', 0x466f7220746865204b6e6f276d6f6e2e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(39, 18, 72, 73, '', 'Soulless', 0x466f722074686520696e636f6e73706963756f757320617373617373696e73206f6620746865206461726b2e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(40, 19, 76, 77, '', 'Immortal', 0x466f7220616c6c20496d6d6f7274616c732e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(41, 19, 78, 79, '', 'Building', 0x466f722074686f73652077686f20637265617465207468652066616e746173746963206172656173206f66206f7572206c6974746c6520776f726c642e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(42, 19, 80, 81, '', 'Scripting', 0x466f722074686520696d6d6f7274616c732077686f2068656c70206d616b65206f757220776f726c64207265616c2e2e2e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(43, 19, 82, 83, '', 'Coding', 0x466f722074686520696d6d6f7274616c732077686f206b65657020757320757020616e642072756e6e696e672e2e2e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(44, 19, 84, 85, '', 'Clan Immortals', 0x4120706c61636520746f206469736375737320726f6c65706c61792e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(45, 19, 86, 87, '', 'Immortal Discussion', 0x466f722063726561746976652069646561732c2062616c616e6365206973737565732c20706f6c69636965732c20616e64206e65772070726f6a656374732e20283130322b20696d6d6f7274616c7329, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(46, 19, 88, 89, '', 'Web/PR Admin', 0x466f722077656220616e64207075626c69632072656c6174696f6e732e2e2e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(47, 19, 90, 91, '', 'Staff', 0x466f7220746865205374616666206f66204b696e736c617965724d55442e20283130342b20696d6d6f7274616c7329, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(48, 19, 92, 93, '', 'Archives', 0x4120706c61636520746f2073746f7265206f75746461746564206f7220696e617070726f70726961746520636f6e74656e742e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(49, 19, 94, 95, '', 'Quests', 0x4120706c61636520666f7220717565737420646576656c6f7065727320746f20636f6e76656e652e, '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1),(50, 19, 96, 97, '', 'KinMMO', '', '', 7, '', '', '', 0, '', '', '', '', 7, '', 0, 1, 0, 0, 0, 0, 0, 0, '', 0, '', '', 48, 0, 1, 0, 1, 0, 0, 0, 7, 7, 1);"
		sqlQuery( sql );
	}	
	addToQueue(f,"Inserting posts into new table...");
	
	f = function() {
		sql = "DELETE FROM phpbb_topics";
	
		sqlQuery( sql );
	}
	addToQueue(f,"Deleting topics in new topic table...");
	
	f = function() {
		sql = "DELETE FROM phpbb_topics_posted";
	
		sqlQuery( sql );
	}
	addToQueue(f,"Deleting phpbb_topics_posted entries...");
	
	f = function() {
		sql = "";
		sql += "INSERT INTO phpbb_topics(\n"
		sql += " phpbb_topics.topic_id,\n"
		sql += " phpbb_topics.forum_id,\n"
		sql += " phpbb_topics.icon_id,\n"
		sql += " phpbb_topics.topic_attachment,\n"
		sql += " phpbb_topics.topic_approved,\n"
		sql += " phpbb_topics.topic_reported,\n"
		sql += " phpbb_topics.topic_title,\n"
		sql += " phpbb_topics.topic_poster,\n"
		sql += " phpbb_topics.topic_time,\n"
		sql += " phpbb_topics.topic_time_limit,\n"
		sql += " phpbb_topics.topic_views,\n"
		sql += " phpbb_topics.topic_replies,\n"
		sql += " phpbb_topics.topic_replies_real,\n"
		sql += " phpbb_topics.topic_status,\n"
		sql += " phpbb_topics.topic_type,\n"
		sql += " phpbb_topics.topic_first_post_id,\n"
		sql += " phpbb_topics.topic_first_poster_name,\n"
		sql += " phpbb_topics.topic_first_poster_colour,\n"
		sql += " phpbb_topics.topic_last_post_id,\n"
		sql += " phpbb_topics.topic_last_poster_id,\n"
		sql += " phpbb_topics.topic_last_poster_name,\n"
		sql += " phpbb_topics.topic_last_poster_colour,\n"
		sql += " phpbb_topics.topic_last_post_subject,\n"
		sql += " phpbb_topics.topic_last_post_time,\n"
		sql += " phpbb_topics.topic_last_view_time,\n"
		sql += " phpbb_topics.topic_moved_id,\n"
		sql += " phpbb_topics.topic_bumped,\n"
		sql += " phpbb_topics.topic_bumper,\n"
		sql += " phpbb_topics.poll_title,\n"
		sql += " phpbb_topics.poll_start,\n"
		sql += " phpbb_topics.poll_length,\n"
		sql += " phpbb_topics.poll_max_options,\n"
		sql += " phpbb_topics.poll_last_vote,\n"
		sql += " phpbb_topics.poll_vote_change) #34\n"
		sql += " SELECT\n"
		sql += " forums_topics.topic_id,\n"
		sql += " forums_topics.forum_id,\n"
		sql += " 0,\n"
		sql += " 0,\n"
		sql += " 1,\n"
		sql += " 0,\n"
		sql += " forums_topics.topic_title,\n"
		sql += " forums_topics.topic_poster,\n"
		sql += " forums_topics.topic_time,\n"
		sql += " 0,\n"
		sql += " forums_topics.topic_views,\n"
		sql += " forums_topics.topic_replies,\n"
		sql += " forums_topics.topic_replies,\n"
		sql += " forums_topics.topic_status,\n"
		sql += " forums_topics.topic_type,\n"
		sql += " forums_topics.topic_first_post_id,\n" //topic_first_post_id (DONE)
		sql += " (SELECT username FROM tempUsers WHERE user_id=forums_topics.topic_poster LIMIT 1),\n" //topic_first_poster_name (DONE)
		sql += " '',\n" //topic_first_poster_colour (DONE)
		sql += " forums_topics.topic_last_post_id,\n"//topic_last_post_id (DONE)
		sql += " 0,\n" //topic_last_poster_id (DONE)
		sql += " '',\n"//topic_last_poster_name (DONE)
		sql += " '',\n"//topic_last_poster_colour (DONE)
		sql += " '',\n"//topic_last_post_subject 
		sql += " 0,\n"//topic_last_post_time
		sql += " 0,\n"
		sql += " 0,\n"
		sql += " 0,\n"
		sql += " 0,\n"
		sql += " forums_vote_desc.vote_text,\n"
		sql += " forums_vote_desc.vote_start,\n"
		sql += " forums_vote_desc.vote_length,\n"
		sql += " 0,\n"
		sql += " 0,\n"
		sql += " 0\n"
		sql += " FROM forums_topics\n"
		sql += " LEFT JOIN forums_vote_desc ON forums_vote_desc.topic_id=forums_topics.topic_id\n"
		
		sqlQuery( sql );
	}
	addToQueue(f,"Copying topics from old table to new table...");
	
	f = function() {
	
		sql = "UPDATE tempUsers SET user_options=230271";
		
		sqlQuery( sql );
	}
	addToQueue(f,"Updating user options...");
	
	f = function() {
	
		var groupToClanNumMapper = new Object();
		
		groupToClanNumMapper[ 12 ] = [17,18,19,20,21,22,23];
		groupToClanNumMapper[ 22 ] = [];
		groupToClanNumMapper[ 14 ] = [14]; //Blademasters
		groupToClanNumMapper[ 8 ] = [3]; //Borderguards
		groupToClanNumMapper[ 21 ] = [8];//Dha'vol
		groupToClanNumMapper[ 18 ] = [9];//Dhjin'nen
		groupToClanNumMapper[ 19 ] = [27];//Dreadguards
		groupToClanNumMapper[ 13 ] = [1];//Gaidin
		groupToClanNumMapper[ 16 ] = [11];//Ghob'hlin
		groupToClanNumMapper[ 24 ] = [10];//Kno'mon
		groupToClanNumMapper[ 17 ] = [7];//Ko'bal
		groupToClanNumMapper[ 20 ] = [28];//Myrddraal
		groupToClanNumMapper[ 9 ] = [2];//Queen's Guards
//		groupToClanNumMapper[  ] = [];//Shadow Council
		groupToClanNumMapper[ 25 ] = [25];//Soulless
		groupToClanNumMapper[ 11 ] = [16];//The White Tower
		groupToClanNumMapper[ 10 ] = [6];//Valon
		groupToClanNumMapper[ 15 ] = [13];//Wolfbrothers
		groupToClanNumMapper[ 26 ] = [ 4 ];//Murandian Cavalry
		
		for(var groupID in groupToClanNumMapper ) {
		
			var vClanIDs = groupToClanNumMapper[ groupID ];
			var sClanIDs = vClanIDs.join(",");
			
			if( vClanIDs.length == 0 ) {
				continue;
			}
			
			sql = "DELETE FROM phpbb_user_group";
			sql += " WHERE phpbb_user_group.group_id=" + groupID;
			
			sqlQuery( sql );
			
			sql = "";
			
			sql += "INSERT INTO phpbb_user_group(";
			sql += " group_id,";
			sql += " user_id,";
			sql += " group_leader,";
			sql += " user_pending";
			sql += " )";
			sql += " SELECT";
			sql += " " + groupID + ",";
			sql += " users.user_id,";
			sql += " 0,";
			sql += " 0";
			sql += " FROM users";
			sql += " LEFT JOIN clans ON users.username=clans.name"
			sql += " WHERE clans.clan IN(" + sClanIDs + ")";
			
			sqlQuery( sql );
		}
		sql = "DELETE FROM phpbb_user_group WHERE group_id=27";
		
		sqlQuery( sql );
		
		sql = "";
	
		sql += "INSERT INTO phpbb_user_group (group_id,user_id,group_leader,user_pending)";
		sql += " SELECT";
		sql += " 27,";
		sql += " users.user_id,";
		sql += " 0,";
		sql += " 0";
		sql += " FROM users WHERE users.level>=100";
		
		sqlQuery( sql );
		
		sql = "DELETE FROM phpbb_user_group WHERE group_id=31";
		
		sqlQuery( sql );
		
		sql = "";
	
		sql += "INSERT INTO phpbb_user_group (group_id,user_id,group_leader,user_pending)";
		sql += " SELECT";
		sql += " 31,";
		sql += " users.user_id,";
		sql += " 0,";
		sql += " 0";
		sql += " FROM users WHERE users.level>=102";
		
		sqlQuery( sql );
	
	}
	f();
	return;
	addToQueue(f,"Adding clanned users to clan forum usergroups...");
	
	f = function() {
	
		var oldToNewForumMapper = new Object();
		
		oldToNewForumMapper[ 1 ] = 2;
		oldToNewForumMapper[ 23 ] = 3;
		oldToNewForumMapper[ 2 ] = 4;
		oldToNewForumMapper[ 3 ] = 5;
		oldToNewForumMapper[ 45 ] = 6;
		
		oldToNewForumMapper[ 4 ] = 7;
		oldToNewForumMapper[ 5 ] = 8;
		oldToNewForumMapper[ 35 ] = 11;
		oldToNewForumMapper[ 6 ] = 12;
		oldToNewForumMapper[ 7 ] = 13;
		oldToNewForumMapper[ 8 ] = 14;
		oldToNewForumMapper[ 9 ] = 15;
		oldToNewForumMapper[ 21 ] = 16;
		
		oldToNewForumMapper[ 10 ] = 20;
		oldToNewForumMapper[ 24 ] = 21;
		oldToNewForumMapper[ 27 ] = 22;
		oldToNewForumMapper[ 28 ] = 23;
		oldToNewForumMapper[ 30 ] = 24;
		oldToNewForumMapper[ 33 ] = 25;
		oldToNewForumMapper[ 41 ] = 26;
		oldToNewForumMapper[ 31 ] = 51;
		oldToNewForumMapper[ 42 ] = 27;
		oldToNewForumMapper[ 26 ] = 28;
		oldToNewForumMapper[ 36 ] = 29;
		oldToNewForumMapper[ 11 ] = 30;
		oldToNewForumMapper[ 12 ] = 31;
		oldToNewForumMapper[ 25 ] = 32;
		oldToNewForumMapper[ 29 ] = 33;
		oldToNewForumMapper[ 34 ] = 34;
		oldToNewForumMapper[ 37 ] = 35;
		oldToNewForumMapper[ 38 ] = 36;
		oldToNewForumMapper[ 39 ] = 37;
		oldToNewForumMapper[ 40 ] = 38;
		oldToNewForumMapper[ 43 ] = 39;
		
		oldToNewForumMapper[ 13 ] = 40;
		oldToNewForumMapper[ 14 ] = 41;
		oldToNewForumMapper[ 15 ] = 42;
		oldToNewForumMapper[ 16 ] = 43;
		oldToNewForumMapper[ 17 ] = 44;
		oldToNewForumMapper[ 18 ] = 45;
		oldToNewForumMapper[ 22 ] = 48;
		oldToNewForumMapper[ 19 ] = 46;
		oldToNewForumMapper[ 44 ] = 50;
		oldToNewForumMapper[ 20 ] = 47;
		oldToNewForumMapper[ 32 ] = 49;
		
		for( var index in oldToNewForumMapper ) {
		
			sql = "UPDATE phpbb_topics SET phpbb_topics.forum_id=" + oldToNewForumMapper[index] + " WHERE phpbb_topics.topic_id IN (SELECT forums_topics.topic_id FROM forums_topics WHERE forums_topics.forum_id=" + index + ")";
			
			sqlQuery( sql );
		
		}
	}
	addToQueue(f,"Assigning phpbb_topics.forum_id to new values...");
	f = function() {
		sql = "DELETE FROM phpbb_posts";
	
		sqlQuery( sql );
	}
	addToQueue(f,"Deleting posts in new post table...");
	f = function() {
		sql = "";
		sql += "INSERT INTO phpbb_posts(\n";
		sql += "post_id,\n";
		sql += "topic_id,\n";
		sql += "forum_id,\n";
		sql += "poster_id,\n";
		sql += "icon_id,\n";
		sql += "poster_ip,\n";
		sql += "post_time,\n";
		sql += "post_approved,\n";
		sql += "post_reported,\n";
		sql += "enable_bbcode,\n";
		sql += "enable_smilies,\n";
		sql += "enable_magic_url,\n";
		sql += "enable_sig,\n";
		sql += "post_username,\n";
		sql += "post_subject,\n";
		sql += "post_text,\n";
		sql += "post_checksum,\n";
		sql += "post_attachment,\n";
		sql += "bbcode_bitfield,\n";
		sql += "bbcode_uid,\n";
		sql += "post_postcount,\n";
		sql += "post_edit_time,\n";
		sql += "post_edit_reason,\n";
		sql += "post_edit_user,\n";
		sql += "post_edit_count,\n";
		sql += "post_edit_locked\n";
		sql += ") SELECT \n";
		sql += "forums_posts.post_id,\n"
		sql += "forums_posts.topic_id,\n";
		sql += "phpbb_topics.forum_id,\n";
		sql += "forums_posts.poster_id,\n";
		sql += "0,\n";
		sql += "forums_posts.poster_ip,\n";
		sql += "forums_posts.post_time,\n";
		sql += "1,\n";
		sql += "0,\n";
		sql += "forums_posts.enable_bbcode,\n";
		sql += "forums_posts.enable_smilies,\n";
		sql += "1,\n"; //enable_magic_url
		sql += "forums_posts.enable_sig,\n";
		sql += "forums_posts.post_username,\n";
		sql += "forums_posts_text.post_subject,\n";
		sql += "forums_posts_text.post_text,\n";
		sql += "'',\n";//post checksum
		sql += "0,\n";
		sql += "'',\n";//bbcode_bitfield
		sql += "forums_posts_text.bbcode_uid,\n";//bbcode_uid
		sql += "1,\n";//post_postcount
		sql += "forums_posts.post_edit_time,\n";//post_edit_time
		sql += "'',\n";//post_edit_reason
		sql += "0,\n";//post_edit_user
		sql += "forums_posts.post_edit_count,\n";//post_edit_count
		sql += "0\n";//post_edit_locked
		sql += "FROM forums_posts\n";
		sql += "LEFT JOIN phpbb_topics ON forums_posts.topic_id=phpbb_topics.topic_id\n"
		sql += "LEFT JOIN forums_posts_text ON forums_posts_text.post_id=forums_posts.post_id\n";
		
		sqlQuery( sql );
	}
	addToQueue(f,"Copying posts from old table to new table...");
	
	f = function() {
		sql = "UPDATE phpbb_forums SET forum_topics=(SELECT COUNT(*) FROM phpbb_topics WHERE phpbb_topics.forum_id=phpbb_forums.forum_id),forum_topics_real=(SELECT COUNT(*) FROM phpbb_topics WHERE phpbb_topics.forum_id=phpbb_forums.forum_id);";
		sqlQuery( sql );
	}
	addToQueue(f,"Updating forum topic counts...");
	
	f = function() {
		sql = "UPDATE phpbb_topics SET phpbb_topics.topic_last_poster_id=(SELECT forums_posts.poster_id FROM forums_posts WHERE forums_posts.topic_id=phpbb_topics.topic_id ORDER BY forums_posts.post_time DESC LIMIT 1)";
		
		sqlQuery( sql );
	}
	addToQueue(f,"Updating topic last poster identification numbers...");
	
	f = function() {
		sql = "UPDATE phpbb_topics SET phpbb_topics.topic_last_poster_name=(SELECT tempUsers.username FROM forums_posts LEFT JOIN tempUsers ON forums_posts.poster_id = tempUsers.user_id WHERE forums_posts.topic_id=phpbb_topics.topic_id ORDER BY forums_posts.post_time DESC LIMIT 1)";
		
		sqlQuery( sql );
	}
	addToQueue(f,"Updating topic last poster usernames...");
	
	f = function() {
		sql = "UPDATE phpbb_topics SET phpbb_topics.topic_last_post_time=(SELECT forums_posts.post_time FROM forums_posts WHERE forums_posts.topic_id=phpbb_topics.topic_id ORDER BY forums_posts.post_time DESC LIMIT 1)";
		sqlQuery( sql );
	}
	addToQueue(f,"Updating topic last post times...");
	
	f = function() {
		sql = "UPDATE phpbb_forums SET phpbb_forums.forum_last_post_id=(SELECT phpbb_posts.post_id FROM phpbb_posts WHERE phpbb_posts.forum_id=phpbb_forums.forum_id ORDER BY phpbb_posts.post_time DESC LIMIT 1);";
		
		sqlQuery( sql );
	}
	addToQueue(f,"Updating forum last post identification numbers...");
	
	f = function() {
		sql = "UPDATE phpbb_forums SET phpbb_forums.forum_last_poster_id=(SELECT phpbb_posts.poster_id FROM phpbb_posts WHERE phpbb_posts.forum_id=phpbb_forums.forum_id ORDER BY phpbb_posts.post_time DESC LIMIT 1);";
		
		sqlQuery( sql );
	}
	addToQueue(f,"Updating forum last poster identification numbers...");
	f = function() {
		sql = "UPDATE phpbb_forums SET phpbb_forums.forum_last_poster_name=(SELECT tempUsers.username FROM phpbb_posts LEFT JOIN tempUsers ON tempUsers.user_id=phpbb_posts.poster_id WHERE phpbb_posts.forum_id=phpbb_forums.forum_id ORDER BY phpbb_posts.post_time DESC LIMIT 1);";
		
		sqlQuery( sql );
	}
	addToQueue(f,"Updating forum last poster usernames...");
	f = function() {
		sql = "UPDATE phpbb_forums SET phpbb_forums.forum_last_post_time=(SELECT phpbb_posts.post_time FROM phpbb_posts WHERE phpbb_posts.forum_id=phpbb_forums.forum_id ORDER BY phpbb_posts.post_time DESC LIMIT 1);";
		
		sqlQuery( sql );
	}
	addToQueue(f,"Updating forum last post times...");
	
	f = function() {
		sql = "UPDATE phpbb_forums SET phpbb_forums.forum_posts=(SELECT COUNT(*) FROM phpbb_posts WHERE phpbb_posts.forum_id=phpbb_forums.forum_id)";
		
		sqlQuery( sql );
	}
	addToQueue(f,"Updating forum post counts...");
	
	f = function() {
		sql = "UPDATE tempUsers SET tempUsers.user_style=2 WHERE user_id=1";
		
		sqlQuery( sql );
	}
//	addToQueue(f,"Updating user styles...");
	f = function() {
	
		sql = "";
		
		sql = "TRUNCATE TABLE phpbb_privmsgs";
		
		sqlQuery( sql );
		
		sql = "";
	
		sql += "INSERT INTO phpbb_privmsgs (";
		sql += " msg_id,";
		sql += " root_level,";
		sql += " author_id,";
		sql += " icon_id,";
		sql += " author_ip,";
		sql += " message_time,";
		sql += " enable_bbcode,";
		sql += " enable_smilies,";
		sql += " enable_magic_url,";
		sql += " enable_sig,";
		sql += " message_subject,";
		sql += " message_text,";
		sql += " message_edit_reason,";
		sql += " message_edit_user,";
		sql += " message_attachment,";
		sql += " bbcode_bitfield,";
		sql += " bbcode_uid,";
		sql += " message_edit_time,";
		sql += " message_edit_count,";
		sql += " to_address,";
		sql += " bcc_address,";
		sql += " message_reported";
		sql += " )";
		sql += " SELECT";
		sql += " forums_privmsgs.privmsgs_id,";
		sql += " 0,";
		sql += " forums_privmsgs.privmsgs_from_userid,";
		sql += " 0,";
		sql += " forums_privmsgs.privmsgs_ip,";
		sql += " forums_privmsgs.privmsgs_date,";
		sql += " 1,";
		sql += " 1,";
		sql += " 1,";
		sql += " 1,";
		sql += " forums_privmsgs.privmsgs_subject,";
		sql += " forums_privmsgs_text.privmsgs_text,";
		sql += " '',";
		sql += " 0,";
		sql += " 0,";
		sql += " '',";
		sql += " forums_privmsgs_text.privmsgs_bbcode_uid,";
		sql += " 0,";
		sql += " 0,";
		sql += " CONCAT('u_',forums_privmsgs.privmsgs_to_userid),";
		sql += " '',";
		sql += " 0";
		sql += " FROM forums_privmsgs";
		sql += " LEFT JOIN forums_privmsgs_text ON forums_privmsgs_text.privmsgs_text_id=forums_privmsgs.privmsgs_id";
		
		sqlQuery( sql );
		
		sql = "TRUNCATE TABLE phpbb_privmsgs_to";
		
		sqlQuery( sql );
		
		sql = "";
		
		sql += "INSERT INTO phpbb_privmsgs_to (";
		sql += " msg_id,";
		sql += " user_id,";
		sql += " author_id,";
		sql += " pm_deleted,";
		sql += " pm_new,";
		sql += " pm_unread,";
		sql += " pm_replied,";
		sql += " pm_marked,";
		sql += " pm_forwarded,";
		sql += " folder_id";
		sql += " )";
		sql += " SELECT";
		sql += " forums_privmsgs.privmsgs_id,";
		sql += " forums_privmsgs.privmsgs_to_userid,";
		sql += " forums_privmsgs.privmsgs_from_userid,";
		sql += " 0,";
		sql += " 0,";
		sql += " 0,";
		sql += " 0,";
		sql += " 0,";
		sql += " 0,";
		sql += " 0";
		sql += " FROM forums_privmsgs";
		
		sqlQuery( sql );
		
		sql = "UPDATE phpbb_privmsgs_to LEFT JOIN forums_privmsgs ON phpbb_privmsgs_to.msg_id=forums_privmsgs.privmsgs_id SET phpbb_privmsgs_to.folder_id=-1 WHERE forums_privmsgs.privmsgs_type=2";
		
		sqlQuery( sql );
		
		sql = "UPDATE phpbb_privmsgs_to SET user_id=author_id WHERE folder_id=-1";
		
		sqlQuery( sql );
	}
	addToQueue(f,"Copying private messages over to new forum tables...");
	
	f = function() {
	
		sql = "UPDATE tempUsers SET user_new_privmsg=(SELECT COUNT(*) FROM phpbb_privmsgs WHERE to_address =CONCAT('u_',tempUsers.user_id))";
		
		sqlQuery( sql );
	
	}
//	addToQueue(f,"Updating user private message counts...");
	
	f = function( vArgs ) {
	
		var vBBCodes = [];
		if( !vArgs || vArgs.length < 2 ) {
			vArgs = [0,1000];
		}
		var offset = vArgs[0], batchSize = vArgs[1];
		
		var pattern;
		var tableSize = 0;
		var tags = new Object();
		tags["quote"] = 0;
		tags["b"] =  1;
		tags["i"] =  2;
		tags["url"] =  3;
		tags["img"] =  4;
		tags["size"] =  5;
		tags["color"] =  6;
		tags["u"] =  7;
		tags["code"] =  8;
		tags["list"] =  9;
		tags["email"] = 10;
		tags["flash"] = 11;
		tags["str"] = 12;
		
		var sql = "SELECT COUNT(*) AS tableSize FROM phpbb_privmsgs";
		var resultSet = sqlQuery( sql );
		
		tableSize = parseInt(resultSet.getRow.get("tableSize"));
		
		var postsUpdated = 0;
		var timeStart = time();
		while( offset < tableSize ) {
		
			sql  = "SELECT phpbb_privmsgs.msg_id, phpbb_privmsgs.message_text, phpbb_privmsgs.bbcode_bitfield, phpbb_privmsgs.bbcode_uid";
			sql +=" FROM phpbb_privmsgs ORDER BY msg_id ASC LIMIT " + offset + "," + batchSize;
//			getRoom(2).echo(sql);
			var resultSet = sqlQuery(sql);
			
			while( resultSet.hasNextRow ) {
				pattern = /(?:\[([a-zA-Z]+).*?:([a-zA-Z0-9]+)\])|(?:\[\/([a-zA-Z]+).*?:([a-zA-Z0-9]+)\])/g;
				++postsUpdated;
				var row;
				var postID;
				var postText;
				var bitfield;
				var bbcodeUID;
				var bbcodeUIDToReplace;
				
				try {
					row = resultSet.getRow;
					postID = row.get("msg_id");
					postText = row.get("message_text");
					bbcodeUID = row.get("bbcode_uid");
					bitfield = new BitField();
					bbcodeUIDToReplace = [];
				} catch(e) {
//					mudLog(constants.BRF,102,"Failed to update post #" + postID);
					continue;
				}
				
				var matches;
				for(matches = pattern.exec(postText);matches;matches = pattern.exec()) {
					
					if( !matches ) {
						break;
					}
					if( matches[ 1 ] ) {
						var tagName = matches[ 1 ];
						thisBBCodeUID = matches[ 2 ];
					}
					else {
						var tagName = matches[ 3 ];
						thisBBCodeUID = matches[ 4 ];
					}
					
					var tagNumber = tags[ tagName ];
//					getRoom(2).echo("This BBCode UID: " + thisBBCodeUID);
					
					if( thisBBCodeUID.length < 8 )
						continue;
					if( arrContains(bbcodeUIDToReplace,thisBBCodeUID) == false ) {
						bbcodeUIDToReplace.push( thisBBCodeUID );
					}
					if( tagNumber == undefined )
						continue;
					
//					mudLog(constants.BRF,104,"Tag: " + tagName);
//					mudLog(constants.BRF,104,"Tag ID: " + tagNumber);
//					getRoom(2).echo("Tag Number: " + tagNumber);
					bitfield.set(tagNumber);
				}
//				mudLog(constants.BRF,104,"Replace With: " + bbcodeUID);
//				mudLog(constants.BRF,104,"BBCode Bitfield: " + bitfield.get_base64());
//				mudLog(constants.BRF,104,"BBCode UIDs to replace: " + bbcodeUIDToReplace);
				while( bbcodeUIDToReplace.length ) {
					var searchTerm = bbcodeUIDToReplace.pop();
					if( searchTerm.length == 8 )
						continue;
					searchTerm = ":" + searchTerm;
					while( postText.search( searchTerm ) != -1 ) {
						postText = postText.replace(searchTerm, ":" + bbcodeUID);
					}
				}
				while( postText.search( "code:1:" ) != -1 ) {
					postText = postText.replace("code:1:","code:");
				}
				while( postText.search( "\"" ) != -1 ) {
					postText = postText.replace("\"","&quot;");
				}
				pattern = /\[quote:(........)(.*?)\]/g;
				
				while( true ) {
				
					var matches = pattern.exec( postText );
					if( !matches ) {
						break;
					}
					
					var uid = matches[1];
					var between = matches[2];
					
					postText = postText.replace( matches[0], "[quote" + between + ":" + uid + "]" );
				
				}
				
//				mudLog(constants.BRF,105,"BitField: " + bitfield.get_base64());
//				mudLog(constants.BRF,105,"New Post Body:\n" + postText);
				sql = "";
				sql += "UPDATE phpbb_privmsgs SET";
				sql += " phpbb_privmsgs.message_text='" + sqlEsc(postText) + "',";
				sql += " phpbb_privmsgs.bbcode_bitfield='" + sqlEsc(bitfield.get_base64()) + "'";
				sql += " WHERE phpbb_privmsgs.msg_id=" + postID;
				
				sqlQuery(sql);
			}
			offset += batchSize;
			setTimeout( 5, arguments.callee, [offset,batchSize] );
			return;
		}
	
	}
	addToQueue(f,"Fixing private message bbcodes...");
	
	f = function() {
	
		sql = "DELETE FROM phpbb_user_group WHERE group_id=2";
		
		sqlQuery( sql );
		
		sql =  "INSERT INTO phpbb_user_group(";
		sql += " group_id,";
		sql += " user_id,";
		sql += " group_leader,";
		sql += " user_pending)";
		sql += " SELECT";
		sql += " 2,";
		sql += " user_id,";
		sql += " 0,";
		sql += " 0";
		sql += " FROM users";
		sql += " WHERE user_id NOT IN(-1,11)";
	
		sqlQuery( sql );
	
	}
	addToQueue(f,"Adding registered users to Registered Users usergroup...");
		
	f = function() {
		
		var vBBCodes = [];
		
		var pattern = /(?:\[([a-zA-Z]+).*?:([a-zA-Z0-9]+)\])|(?:\[\/([a-zA-Z]+).*?:([a-zA-Z0-9]+)\])/g;
		var offset = 0;
		var batchSize = 1000;
		var tableSize = 0;
		var tags = new Object();
		tags["quote"] = 0;
		tags["b"] =  1;
		tags["i"] =  2;
		tags["url"] =  3;
		tags["img"] =  4;
		tags["size"] =  5;
		tags["color"] =  6;
		tags["u"] =  7;
		tags["code"] =  8;
		tags["list"] =  9;
		tags["email"] = 10;
		tags["flash"] = 11;
		tags["str"] = 12;
		
		var sql = "SELECT COUNT(*) AS tableSize FROM phpbb_posts";
		var resultSet = sqlQuery( sql );
		
		tableSize = parseInt(resultSet.getRow.get("tableSize"));
		
		var postsUpdated = 0;
		while( offset < tableSize ) {
		
			sql  = "SELECT phpbb_posts.post_id, phpbb_posts.post_text, phpbb_posts.bbcode_bitfield, phpbb_posts.bbcode_uid";
			sql +=" FROM phpbb_posts LIMIT " + offset + "," + batchSize;
//			getRoom(2).echo(sql);
			var resultSet = sqlQuery(sql);
			
			while( resultSet.hasNextRow ) {
				++postsUpdated;
				var row;
				var postID;
				var postText;
				var bitfield;
				var bbcodeUID;
				var bbcodeUIDToReplace;
				
				try {
					row = resultSet.getRow;
					postID = row.get("post_id");
					global.lastEditPost = postID;
					postText = row.get("post_text");
					bbcodeUID = row.get("bbcode_uid");
					bitfield = new BitField();
					bbcodeUIDToReplace = [];
				} catch(e) {
//					mudLog(constants.BRF,102,"Failed to update post #" + postID);
					continue;
				}
				
				pattern = /(?:\[([a-zA-Z]+).*?:([a-zA-Z0-9]+)\])|(?:\[\/([a-zA-Z]+).*?:([a-zA-Z0-9]+)\])/g;
				while( true ) {
					var matches = pattern.exec(postText);
					if( !matches ) {
						break;
					}
					if( matches[ 1 ] ) {
						var tagName = matches[ 1 ];
						thisBBCodeUID = matches[ 2 ];
					}
					else {
						var tagName = matches[ 3 ];
						thisBBCodeUID = matches[ 4 ];
					}
					
					var tagNumber = tags[ tagName ];
//					getRoom(2).echo("This BBCode UID: " + thisBBCodeUID);
					
					if( thisBBCodeUID.length < 8 )
						continue;
					if( arrContains(bbcodeUIDToReplace,thisBBCodeUID) == false ) {
						bbcodeUIDToReplace.push( thisBBCodeUID );
					}
					if( tagNumber == undefined )
						continue;
					
//					mudLog(constants.BRF,104,"Tag: " + tagName);
//					mudLog(constants.BRF,104,"Tag ID: " + tagNumber);
//					getRoom(2).echo("Tag Number: " + tagNumber);
					bitfield.set(tagNumber);
				}
//				mudLog(constants.BRF,104,"Replace With: " + bbcodeUID);
//				mudLog(constants.BRF,104,"BBCode Bitfield: " + bitfield.get_base64());
//				mudLog(constants.BRF,104,"BBCode UIDs to replace: " + bbcodeUIDToReplace);
				
				while( bbcodeUIDToReplace.length ) {
					var searchTerm = bbcodeUIDToReplace.pop();
					if( searchTerm.length == 8 )
						continue;
					searchTerm = ":" + searchTerm;
					while( postText.search( searchTerm ) != -1 ) {
						postText = postText.replace(searchTerm, ":" + bbcodeUID);
					}
				}
				while( postText.search( "code:1:" ) != -1 ) {
					postText = postText.replace("code:1:","code:");
				}
				while( postText.search( "\"" ) != -1 ) {
					postText = postText.replace("\"","&quot;");
				}
				pattern = /\[quote:(........)(.*?)\]/g;
				
				while( true ) {
				
					var matches = pattern.exec( postText );
					if( !matches ) {
						break;
					}
					
					var uid = matches[1];
					var between = matches[2];
					
					postText = postText.replace( matches[0], "[quote" + between + ":" + uid + "]" );
				
				}
//				mudLog(constants.BRF,105,"BitField: " + bitfield.get_base64());
//				mudLog(constants.BRF,105,"New Post Body:\n" + postText);
//				return;
				sql = "";
				sql += "UPDATE phpbb_posts SET";
				sql += " phpbb_posts.post_text='" + sqlEsc(postText) + "',";
				sql += " phpbb_posts.bbcode_bitfield='" + sqlEsc(bitfield.get_base64()) + "'";
				sql += " WHERE phpbb_posts.post_id=" + postID;
				
				sqlQuery(sql);				
			}
			offset += batchSize
		}
	}
	addToQueue(f,"Fixing broken bbcode tags...");
	
	function processQueue( vArgs ) {
		var queue = vArgs[0];
	
		var queueObject = queue.pop();
		
		mudLog(constants.BRF,102,queueObject.message);
		
		setTimeout(2,queueObject.handler,[]);
		if( queue.length ) {
			setTimeout(5,arguments.callee,[queue]);
		}
		else {
			var f = function() {
				mudLog(constants.BRF,102,"Forum upgrade( Script #114 ) complete.");
			}
			setTimeout(5,f,[]);
		}
	}
	queue.reverse();
	processQueue( [queue] );
	
	

};

