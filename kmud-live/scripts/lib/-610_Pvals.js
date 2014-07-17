var PVALS_TABLE = "pvals";
/****
  *
  * Pvals is stored in global.pvals, which is an associative array.
  * The key of the array is a string of the format: <O|C>~=~<ID>~=~sActualKey
  *    For example: C~=~123456~=~ragePoints
  * The value is an object with two properties: the string representing the actual value, and a boolean.
  * The boolean will be true if the pval needs to be saved, and false if not.
  *
  */
function numberOfPvals()
{
	var i = 0;
	for(var property in global.pvals) {
		++i;
	}
	return i;
}
function loadAllPvals()
{
	sqlQuery( "DELETE FROM " + PVALS_TABLE + " WHERE owner_type='O' AND owner_id NOT IN(SELECT id FROM objects);" );
	sqlQuery( "DELETE FROM " + PVALS_TABLE + " WHERE owner_type='C' AND owner_id NOT IN(SELECT user_id FROM users);" );
	var sQuery = "SELECT * FROM " + PVALS_TABLE + ";";
	var rs = sqlQuery(sQuery);
	
	global.pvals = new Object();
	
	while( rs.hasNextRow ) {
		var row = rs.getRow;
		var sOwnerType = row.get("owner_type");
		var sOwnerID = row.get("owner_id");
		var sValue = row.get("value");
		var sKey = row.get("sKey");
		var sFullKey = sOwnerType + "~=~" + sOwnerID + "~=~" + sKey;
		
		var oValueObject = new Object();
		oValueObject.value = sValue;
		oValueObject.needsSave = false;
		
//		mudLog(constants.BRF, 100, sFullKey);
		global.pvals[ sFullKey ] = oValueObject;
	}
	
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
			+ "sKey='" + sqlEsc(sKey) + "', "
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
	
	var pval = global.pvals[ sFullKey ];
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
	var pval = global.pvals[ sFullKey ];
	
	if( !pval ) {
		var newPval = true;
		pval = new Object();
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
		global.pvals[ sFullKey ] = pval;
		global.pvals[ sFullKey ].needsDelete = false;
//		mudLog(constants.BRF, 101, global.pvals[ sFullKey ]);
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
	var pval = global.pvals[ sFullKey ];
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
	
	delete global.pvals[ sFullKey ];
}
function savePvalsInNeedOfSaving()
{
	if( !global.pvals )
		return;
	for(var sFullKey in global.pvals) {
		var pval = global.pvals[ sFullKey ];
		if( pval.needsDelete ) {
			deletePval( sFullKey, pval );
		}
		else if( pval.needsSave ) {
			savePval( sFullKey, pval );
		}
	}
}
