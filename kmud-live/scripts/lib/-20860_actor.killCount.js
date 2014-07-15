//Alder
//November 2010
/**********************************************
 * Temporary killcount getter for JSCharacter *
 **********************************************/
 JSCharacter.prototype.killCount = function() {
	if ( this.vnum == -1 ) {
		var rs = sqlQuery("SELECT pks FROM users WHERE user_id="+sqlEsc(this.id)+";");
		return parseInt(rs.getRow.get("pks"));
	}
}
