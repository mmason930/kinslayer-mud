function addUserToForumGroup( iUserID, iGroupID ) {
	
	var sql = "INSERT IGNORE INTO phpbb_user_group("
	        + " group_id,"
			+ " user_id,"
			+ " group_leader,"
			+ " user_pending"
			+ ") VALUES ( "
			+ iGroupID + ","
			+ iUserID + ","
			+ "0,"
			+ "0"
			+ ");";
			
	sqlQuery( sql );
}
function removeUserFromForumGroup( iUserID, iGroupID ) {
	
	var sql = "DELETE FROM phpbb_user_group "
	        + "WHERE user_id=" + parseInt(iUserID) + " "
			+ "AND group_id=" + parseInt(iGroupID);
	
	sqlQuery( sql );
}
function getForumGroupIDFromClanVnum( iClanVnum ) {
	switch( iClanVnum ) {
		
		case constants.CLAN_GAIDIN: return 13;
		case constants.CLAN_ANDORAN: return 9;
		case constants.CLAN_SHIENARAN: return 8;
		case constants.CLAN_MURANDIAN: return 26;
		case constants.CLAN_KOBOL: return 17;
		case constants.CLAN_GHOBHLIN: return 16;
		case constants.CLAN_WOLFBROTHER: return 15;
		case constants.CLAN_BLADEMASTERS: return 14;
		case constants.CLAN_BLACK_TOWER: return 35;
		case constants.CLAN_WHITE_TOWER: return 11;
		case constants.CLAN_SOULLESS: return 25;
		case constants.CLAN_WOLFBROTHER: return 15;
		case constants.CLAN_DREADGUARDS: return 19;
		case constants.CLAN_VALON: return 10;
		case constants.CLAN_MYRDDRAAL: return 20;
		case constants.CLAN_GRAY_AJAH:
		case constants.CLAN_RED_AJAH:
		case constants.CLAN_YELLOW_AJAH:
		case constants.CLAN_BROWN_AJAH:
		case constants.CLAN_WHITE_AJAH:
		case constants.CLAN_GREEN_AJAH:
		case constants.CLAN_BLUE_AJAH:
			return  12;
		default: return undefined;
	}
}
function createForumPost( iPosterID, iThreadID, sPostBody )
{
	iThreadID = sqlEsc(iThreadID);
	sPostBody = sqlEsc( sPostBody );
	sQuery = "INSERT INTO forums_posts (`topic_id`,`forum_id`,`poster_id`,`post_time`,`poster_ip`,`post_username`,`enable_bbcode`,`enable_html`,`enable_smilies`,`enable_sig`) VALUES(";
	sQuery += "'" + iThreadID + "',";
	sQuery += "(SELECT forum_id FROM forums_topics t WHERE t.topic_id='" + iThreadID + "' LIMIT 1),";
	sQuery += "'" + iPosterID + "',";
	sQuery += "'" + time() + "',";
	sQuery += "'" + "127.0.0.1" + "',";
	sQuery += "(SELECT u.username FROM users u WHERE u.user_id='" + iPosterID + "' LIMIT 1)" + ",";
	sQuery += "'1',";
	sQuery += "'1',";
	sQuery += "'1',";
	sQuery += "'1'";
	sQuery += ");";
	
	sqlQuery( sQuery );
	var iNewPostID = sqlInsertID();
	
//	getRoom(1).echo( sQuery );
	
	sQuery = "INSERT INTO forums_posts_text(`post_id`,`bbcode_uid`,`post_subject`,`post_text`) VALUES(";
	sQuery += "'" + iNewPostID + "',";
	sQuery += "'" + "" + "',";
	sQuery += "'" + "" + "',";
	sQuery += "'" + sPostBody + "'";
	sQuery += ");";
	
	sqlQuery( sQuery );
	
	sQuery = "UPDATE forums_topics SET topic_replies=topic_replies+1, topic_last_post_id='" + iNewPostID + "' WHERE topic_id='" + iThreadID + "';";
	sqlQuery( sQuery );
	
	sQuery = "UPDATE forums_forums SET forum_last_post_id='" + iNewPostID + "' WHERE forum_id=(SELECT t.forum_id FROM forums_topics t WHERE t.topic_id='" + iThreadID + "');";
	sqlQuery( sQuery );
	sqlQuery("UPDATE forums_forums SET forum_posts=forum_posts+1 WHERE forum_id=(SELECT forum_id FROM forums_topics WHERE topic_id=" + iThreadID + ");");
	sqlQuery("UPDATE users SET user_posts=user_posts+1 WHERE user_id=" + iPosterID + ";");
	
	return iNewPostID;
}
function removeForumPost( iPostID )
{
	iPostID = sqlEsc( iPostID );
	var iThreadID = -1;
	var iForumID = -1;
	sqlQuery("UPDATE users SET user_posts=user_posts-1 WHERE user_id=(SELECT poster_id FROM forums_posts WHERE post_id=" + iPostID + ");");
	var rs = sqlQuery("SELECT topic_id,forum_id FROM forums_posts WHERE post_id=" + iPostID + ";");
	if( rs.hasNextRow ) {
		var row = rs.getRow;
		iThreadID = parseInt( row.get("topic_id") );
		iForumID = parseInt( row.get("forum_id") );
	}
	
	sqlQuery("DELETE FROM forums_posts_text WHERE post_id=" + iPostID + ";");
	sqlQuery("DELETE FROM forums_search_wordmatch WHERE post_id=" + iPostID + ";");
	sqlQuery("DELETE FROM forums_posts WHERE post_id=" + iPostID + ";");
	if( iThreadID != -1 ) {
		sqlQuery("UPDATE forums_topics t SET t.topic_replies=t.topic_replies-1, t.topic_first_post_id=(SELECT p.post_id FROM forums_posts p WHERE p.topic_id=" + iThreadID + " ORDER BY post_time ASC LIMIT 1), t.topic_last_post_id=(SELECT p.post_id FROM forums_posts p WHERE p.topic_id=" + iThreadID + " ORDER BY post_time DESC LIMIT 1) WHERE topic_id=" + iThreadID + ";");
	}
	if( iForumID != -1 ) {
		sqlQuery("UPDATE forums_forums f SET f.forum_posts=f.forum_posts-1, f.forum_last_post_id=(SELECT p.post_id FROM forums_posts p WHERE p.forum_id=" + iForumID + " ORDER BY p.post_time DESC LIMIT 1) WHERE f.forum_id=" + iForumID + ";");
	}
}
function sendPM( iFromID, iToID, sSubject, sMsgBody ) {
	var msgID = sqlQuery("SELECT privmsgs_id FROM forums_privmsgs ORDER BY privmsgs_id DESC;").getRow.get("privmsgs_id");
	
	if( !isNumber(msgID) ) {
		mudLog(3,100,"Error sending Private Message; Cannot retrieve new privmsgs_id.");
		return;
	}
	msgID++;
	msgID = sqlEsc(msgID);
	iFromID = sqlEsc(iFromID);
	iToID = sqlEsc(iToID);
	sSubject = sqlEsc(sSubject);
	sMsgBody = sqlEsc(sMsgBody);
	
	var curTime = sqlEsc(time());
	
	/** msg type of 0 means that it shows in the recipients inbox **/
	/** msg type of 2 means that it shows in the senders sentbox **/
	sqlQuery("INSERT INTO forums_privmsgs (`privmsgs_id`, `privmsgs_type`, `privmsgs_subject`,`privmsgs_from_userid`,`privmsgs_to_userid`,`privmsgs_date`,`privmsgs_ip`,`privmsgs_enable_html`,`privmsgs_enable_smilies`,`privmsgs_attach_sig`) VALUES('" + msgID + "', '0', '" + sSubject + "', '" + iFromID + "', '" + iToID + "', '" + curTime + "', 'local', '1', '0', '1');");
	sqlQuery("INSERT INTO forums_privmsgs_text (`privmsgs_text_id`, `privmsgs_text`) VALUES('" + msgID + "', '" + sMsgBody + "');");
	msgID++;
	msgID = sqlEsc(msgID);
	sqlQuery("INSERT INTO forums_privmsgs (`privmsgs_id`, `privmsgs_type`, `privmsgs_subject`,`privmsgs_from_userid`,`privmsgs_to_userid`,`privmsgs_date`,`privmsgs_ip`,`privmsgs_enable_html`,`privmsgs_enable_smilies`,`privmsgs_attach_sig`) VALUES('" + msgID + "', '2', '" + sSubject + "', '" + iFromID + "', '" + iToID + "', '" + curTime + "', 'local', '1', '0', '1');");
	sqlQuery("INSERT INTO forums_privmsgs_text (`privmsgs_text_id`, `privmsgs_text`) VALUES('" + msgID + "', '" + sMsgBody + "');");
}
