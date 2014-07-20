var script1255 = function(self, actor, here, args, extra) {
	/*********************
	 *
	 * Black Tower quest interface for encrypted well passwords.
	 *  --- User must find passage from scroll to complete the quest.
	 *********************/
	function _strEncode ( str )
	{
		var nStr = "";
		var code, base = 65, top = 122;
		for(var i = 0;i < str.length;++i) {
			if( str.charCodeAt( i ) < base || str.charCodeAt( i ) > top )
				continue;
			code = ((str.charCodeAt( i ) - base) + 15) % (top-base);
			nStr += String.fromCharCode( code + base );
		}
		return nStr;
	}
	var vArgs = getArgList( args );
	function _actDisable( cA ) {
		cA.extract = function() {}
	}
	/**** Disable the script's extraction reaction if they use the right passage. ***/
	if( _strEncode(vArgs[0]) == "vtJCOFKHvtEKJ" ){ _actDisable(self); _block; return; }
	if( vArgs.length < 2 ) {
		return;
	}
	/*** Provide the functionality to make this happen ***/
	function _furtherAction( ca ) {
		//...
	}
	function _wellReaction( cA ) {
		if( cA.inClan( 15 ) ) {
			_furtherAction( cA );//Saving roll basically... Not necessary but something to add in the future.
		}
		cA.extract();
	}
	var validTargets = ["GORLERGKLFGDSE", "LFQSTW", "EMOREMlGFHKE", "DHODRELSDF", "RGMKDGMDFG"];
	var validTokens = ["ODSIJKLDFSFE", "UZWLJ", "UZW", "P", "PN", "PNQ", "PNQQ", "UZWL", "EKJNKGFDISJEKLR"];
	//Check to see if the well command was put in by the player...
	var tEncoding = _strEncode( vArgs[1].toLowerCase() );
	var cEncoding = _strEncode( vArgs[0].toLowerCase() );
	if( tEncoding == "well" ) {
		//User tried putting raw input in. Notify them of the problem.
		actor.questSend( "Try using the encoded message from the scroll." );
		_block;
		return;
	}
	if( tEncoding == "stone" ) {
		actor.questSend( "Try using the encoded message from the scroll." );
		_block;
		return;
	}
	//Check to see if the encoding was put in improperly...
	tEncoding = _strEncode( tEncoding );
	cEncoding = _strEncode( cEncoding );
	//Double encoding is instant death. These codes are found on the black passage.
	var testA = false, testB = false;
	for (var _autoKey in validTokens ) {
		var sC = validTokens [_autoKey];
		if( cEncoding == sC ) {
			testA = true;
			break;
		}
	}
	for (var _autoKey in validTargets ) {
		var sT = validTargets [_autoKey];
		if( tEncoding == sT ) {
			testB = true;
			break;
		}
	}
	//We need a reaction if both tests passed.
	if( testA && testB ) {
		_wellReaction( actor );
		_block;
	}
	_noblock;
	/**** TODO: Finish the completion portion of the script ****/
}