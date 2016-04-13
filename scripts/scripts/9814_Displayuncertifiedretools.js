var script9814 = function(self, actor, here, args, extra) {
	// Alder
	// May 2011
	/** Display all uncertified retools **/
	// _block;
	var vArgs = getArgList(args);
	if ( !str_cmp(vArgs[0], "check") && !str_cmp(vArgs[1], "retools") ) {
		_block;
		var query = "SELECT * FROM pvals WHERE owner_type='O' && sKey LIKE 'featRetool%' ORDER BY owner_id;";
		var result = sqlQuery(query);
		var certify = [];
		var i = 0;	// Indicates retool entry number
		var j = 0;	// Tracks iterations
		getCharCols(actor);
		while ( result.hasNextRow ) {
			var row = result.getRow;
			var id = row.get("owner_id");
			var retoolArr = getArgList(row.get("sKey"));	// "featRetool TYPE field"
			var itemType = retoolArr[1];
			var field = retoolArr[2];
			var info = row.get("value");
			var str = "";
			if ( j % 3 == 0 ) {
				i++;
			}
			if ( j % 3 == 0 ) {
				str += grn+i+nrm+") "+id+" - "+itemType+"\n";
				certify.push("DELETE FROM pvals WHERE owner_type='O' && owner_id='"+sqlEsc(id)+"' && sKey LIKE 'featRetool%';");
			}
			str += "\t"+strPadding(field+": "," ",11,"right")+"\""+info+"\"";
			actor.send(str);
			j++;
		}
		self.certify = certify;
		actor.send("\nAll retools displayed. To certify a retool, type: certify <#>");
		return;
	}
	if ( !str_cmp(vArgs[0], "certify") && isNumber(vArgs[1]) ) {
		_block;
		var i = parseInt(vArgs[1]);
		if ( i < 1 || i > self.certify.length ) {
			actor.send("Input a value within the bounds specified.");
			return;
		}
		sqlQuery(self.certify.splice(i-1,1));
		actor.send("Retool certified, deleted from database.");
	}
}