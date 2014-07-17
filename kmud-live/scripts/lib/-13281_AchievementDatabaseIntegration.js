//Alder
//May 2010
//Achievement Database Integration
var ACH_DATA_TABLE = "achievement_data";
var ACH_EXECS_TABLE = "achievement_executions";
var ACH_TAGS_TABLE = "achievement_tags";
function loadAllAchievements() {
	mudLog(constants.BRF, 102, "Loading achievements...");
	var sQuery;
	var vAchievements = [];
	
	sQuery = "SELECT * FROM " + ACH_DATA_TABLE + " ORDER BY vnum ASC;";
	var rs = sqlQuery( sQuery );
	
	sQuery = "SELECT * FROM " + ACH_EXECS_TABLE + " ORDER BY achievement_vnum ASC,position ASC";
	var rsExecutions = sqlQuery( sQuery );
	
	sQuery = "SELECT * FROM " + ACH_TAGS_TABLE + " ORDER BY achievement_vnum ASC";
	var rsTags = sqlQuery( sQuery );
	
	while( rs.hasNextRow ) {
		var row = rs.getRow;
		var ach = new Object();
		
		ach.achVnum = parseInt( row.get("vnum") );
		ach.achName = row.get("name");
		ach.achDesc = row.get("description");
		ach.achHide = parseInt(row.get("hidden"));
		ach.achFeatPoints = parseInt(row.get("feat_points"));
		ach.alwaysAwardFeatPoints = Boolean(parseInt(row.get("always_award")));
		ach.imageFileName = row.get("image_file_name");
		ach.achExecutions = [];
		ach.achTags = [];
		////Load Executions\\\\
		while ( rsExecutions.hasNextRow ) {
			var row = rsExecutions.peekRow;
			if ( parseInt(row.get("achievement_vnum")) == ach.achVnum ) {
				ach.achExecutions.push(row.get("execution"));
			}
			else break;
			rsExecutions.skipRow();
		}
		////Load Tags\\\\
		while ( rsTags.hasNextRow ) {
			var row = rsTags.peekRow;
			if ( parseInt(row.get("achievement_vnum")) == ach.achVnum ) {
				ach.achTags.push(row.get("tag"));
			}
			else break;
			rsTags.skipRow();
		}
		vAchievements.push(ach);
	}
	function fSort(a,b) {
	if ( a.achVnum < b.achVnum )
		return -1;
	else
		return 1;
	}
	global.vAchievements = vAchievements.sort(fSort);
	mudLog(constants.BRF, 102, "Achievements loaded: " + vAchievements.length );
}
function saveAllAchievements()
{
	mudLog(constants.BRF, 102, "Saving achievements...");
	for(var i = 0;i < global.vAchievements.length;++i)
	{
		saveAchievement( global.vAchievements[ i ] );
	}
	mudLog(constants.BRF, 102, "Achievements saved.");
}
function saveAchievement( ach ) {
	var sQuery;
	var result = sqlQuery("SELECT 1 FROM "+ACH_DATA_TABLE+" WHERE vnum="+sqlEsc(ach.achVnum)+";");
	
	if ( !result.hasNextRow ) {
		/***Achievement is not saved to database, add it***/
		sQuery = "INSERT INTO " + ACH_DATA_TABLE + " ("
		           + "   `vnum`,"
			   + "   `feat_points`,"
			   + "   `always_award`,"
			   + "   `hidden`,"
			   + "   `name`,"
			   + "   `description`,"
			   + "   `image_file_name`"
			   + " ) VALUES ("
			   + parseInt(ach.achVnum) + ","
			   + parseInt(ach.achFeatPoints) + ","
			   + parseInt(Number(ach.alwaysAwardFeatPoints)) + ","
			   + parseInt(ach.achHide ? 1 : 0) + ",'"
			   + sqlEscapeQuoteString(ach.achName)
			   + sqlEscapeQuoteString(ach.achDesc) + ","
			   + sqlEscapeQuoteString(ach.imageFileName)
			   + " )";
			   
		sqlQuery( sQuery );
	}
	else {
		/***Achievement is already saved, update***/
		sQuery = " UPDATE " + ACH_DATA_TABLE + " SET"
		            + "   feat_points = " + parseInt(ach.achFeatPoints) + ","
			    + "   always_award = " + parseInt(Number(ach.alwaysAwardFeatPoints)) + ","
			    + "   hidden = " + (ach.achHide ? 1 : 0) + ","
			    + "   name = " + sqlEscapeQuoteString(ach.achName) + ","
			    + "   description = " + sqlEscapeQuoteString(ach.achDesc) + ","
			    + "   image_file_name = " + sqlEscapeQuoteString(ach.imageFileName)
			    + " WHERE vnum = " + parseInt(ach.achVnum);
			    
		sqlQuery( sQuery );
	}
	/****Executions****/
	sqlQuery("DELETE FROM "+ACH_EXECS_TABLE+" WHERE achievement_vnum='"+sqlEsc(ach.achVnum)+"';");
	for ( var i = 0; i < ach.achExecutions.length; i++ ) {
		sqlQuery("INSERT INTO "+ACH_EXECS_TABLE+" (`achievement_vnum`,`execution`,`position`) VALUES('"+sqlEsc(ach.achVnum)+"','"+sqlEsc(ach.achExecutions[i])+"','"+(i+1)+"');");
	}
	/****Tags****/
	sqlQuery("DELETE FROM "+ACH_TAGS_TABLE+" WHERE achievement_vnum='"+sqlEsc(ach.achVnum)+"';");
	for ( var i = 0; i < ach.achTags.length; i++ ) {
		sqlQuery("INSERT INTO "+ACH_TAGS_TABLE+" (`achievement_vnum`,`tag`) VALUES('"+ach.achVnum+"','"+ach.achTags[i]+"');");
	}
	mudLog(3,102,"Achievement "+ach.achVnum+" saved to database");
}
function deleteAchievementFromDatabase( ach, bShowMsg )
{
	if( !ach ) return;
	if( ach.achVnum == undefined ) return;
	sqlQuery( "DELETE FROM " + ACH_DATA_TABLE     + " WHERE vnum = " + parseInt(ach.achVnum));
	sqlQuery( "DELETE FROM " + ACH_EXECS_TABLE    + " WHERE achievement_vnum='" + sqlEsc(ach.achVnum) + "';" );
	sqlQuery( "DELETE FROM " + ACH_TAGS_TABLE     + " WHERE achievement_vnum='" + sqlEsc(ach.achVnum) + "';" );
	if ( bShowMsg == true )
		mudLog(3,102,"Achievement "+ach.achVnum+" deleted from database");
}

