let PVALS_TABLE = "pvals";
let MAX_VALUE_LENGTH = 64;
/****
  *
  * Pvals is stored in global.pvals, which is a JS map.
  * The key of the array is a string of the format: <O|C>~=~<ID>~=~sActualKey
  *    For example: C~=~123456~=~ragePoints
  * The value is an object with two properties: the string representing the actual value, and a boolean.
  * The boolean will be true if the pval needs to be saved, and false if not.
  *
  */
function numberOfPvals()
{
	return global.pvals.size;
}
function loadAllPvals()
{
	//mudLog(constants.BRF, 102, "loadAllPvals 001");
	//sqlQuery( "DELETE FROM " + PVALS_TABLE + " WHERE owner_type='O' AND owner_id NOT IN(SELECT id FROM objects);" );
	//mudLog(constants.BRF, 102, "loadAllPvals 002");
	//sqlQuery( "DELETE FROM " + PVALS_TABLE + " WHERE owner_type='C' AND owner_id NOT IN(SELECT user_id FROM users);" );
	mudLog(constants.BRF, 102, "loadAllPvals 003");
	var sQuery = "SELECT * FROM " + PVALS_TABLE + ";";
	var rs = sqlQuery(sQuery);

	mudLog(constants.BRF, 102, "loadAllPvals 004");

	global.pvals = new Map();
	while( rs.hasNextRow ) {
		const row = rs.getRow;
		const sOwnerType = row.get("owner_type");
		const sOwnerID = row.get("owner_id");
		const sValue = row.get("value");
		const sKey = row.get("sKey");
		const sFullKey = sOwnerType + "~=~" + sOwnerID + "~=~" + sKey;

		const oValueObject = {};
		oValueObject.value = sValue;
		oValueObject.needsSave = false;

//		mudLog(constants.BRF, 100, sFullKey);
		global.pvals.set(sFullKey, oValueObject);
	}
	mudLog(constants.BRF, 102, "loadAllPvals 005");
	
//	mudLog(constants.BRF, 102, "pvals loaded.");
}
function savePval( sFullKey, pvalObject ) {
	if( !pvalObject || !sFullKey )
		return;
	var vElements = sFullKey.split("~=~");
	
	var sType = vElements[0];
	var sID = vElements[1];
	var sKey = vElements[2];
	var sValue = pvalObject.value;
	var sQuery = "REPLACE DELAYED INTO " + PVALS_TABLE + " SET "
			+ "owner_type='" + sqlEsc(sType) + "', "
			+ "owner_id='" + sqlEsc(sID) + "', "
			+ "sKey='" + sqlEsc(sKey == null ? null : sKey.substring(0, MAX_VALUE_LENGTH)) + "', "
			+ "value='" + sqlEsc(sValue) + "';";
			
	sqlQuery( sQuery );
	pvalObject.needsSave = false;
	pvalObject.needsDelete = false;
}
JSCharacter.prototype.getPval =
function( sKeyName )
{
	return getPval( "C", this.id, sKeyName );
};
JSObject.prototype.getPval =
function( sKeyName )
{
//	mudLog(constants.BRF, 100, "Test");
	return getPval( "O", this.id, sKeyName );
};
function getPval( sOwnerType, sOwnerID, sKeyName )
{
	if( !global.pvals )
		return null;
	
	var sFullKey = sOwnerType + "~=~" + sOwnerID + "~=~" + sKeyName;
	
//	mudLog(constants.BRF, 101, "Loading pval: " + sFullKey);
	
	var pval = global.pvals.get(sFullKey);
	if( pval && !pval.needsDelete ) {
		return pval.value;
	}
	return null;
}
JSCharacter.prototype.setPval =
function( sKeyName, sValue, bInstant )
{
	return setPval( "C", this.id, sKeyName, sValue, bInstant );
};
JSObject.prototype.setPval =
function( sKeyName, sValue, bInstant )
{
	return setPval( "O", this.id, sKeyName, sValue, bInstant );
};
function setPval( sOwnerType, sOwnerID, sKeyName, sValue, bInstant )
{
	if( !global.pvals )
		return;
	var sFullKey = sOwnerType + "~=~" + sOwnerID + "~=~" + sKeyName;
	var pval = global.pvals.get(sFullKey);
	
	if( !pval ) {
		var newPval = true;
		pval = {};
	}
	pval.value = sValue+"";
	if( bInstant == true ) {
		pval.needsSave = false;
		savePval( sFullKey, pval );
	}
	else
		pval.needsSave = true;
	pval.needsDelete = false;
	if( newPval ) {
		global.pvals.set(sFullKey, pval);
		global.pvals.get(sFullKey).needsDelete = false;
//		mudLog(constants.BRF, 101, global.pvals.get(sFullKey));
	}
//	mudLog(constants.BRF, 101, "Saving pval: " + sFullKey);
}
JSCharacter.prototype.deletePval =
function( sKeyName, bInstant )
{
	setPvalToDelete( "C", this.id, sKeyName, bInstant );
};
JSObject.prototype.deletePval =
function( sKeyName, bInstant )
{
	setPvalToDelete( "O", this.id, sKeyName, bInstant );
};
function setPvalToDelete( sOwnerType, sOwnerID, sKeyName, bInstant )
{
	var sFullKey = sOwnerType + "~=~" + sOwnerID + "~=~" + sKeyName;
	var pval = global.pvals.get(sFullKey);
	if( pval ) {
		if( bInstant )
			deletePval( sFullKey, pval );
		else
			pval.needsDelete = true;
	}
}
function deletePval( sFullKey, pval )
{
	var vElements = sFullKey.split("~=~");
	
	var sType = vElements[0];
	var sID = vElements[1];
	var sKey = vElements[2];
	var sValue = pval.value;
	
	sQuery = "DELETE FROM pvals WHERE "
	+ "owner_type='" + sqlEsc(sType) + "' AND "
	+ "owner_id='" + sqlEsc(sID) + "' AND "
	+ "sKey='" + sqlEsc(sKey) + "';";
	
	sqlQuery( sQuery );
	
	global.pvals.delete(sFullKey);
}
function savePvalsInNeedOfSaving()
{
	if( !global.pvals )
		return;
	for (const [sFullKey, pval] of global.pvals) {
		if (pval.needsDelete) {
			deletePval(sFullKey, pval);
		} else if (pval.needsSave) {
			savePval(sFullKey, pval);
		}
	}
}
