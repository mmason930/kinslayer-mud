//Alder
//November 2010
/***********************************************************
 * Gets/sets players feat points                           *
 *														   *
 ***********************************************************/
	// Get featPoints
	JSCharacter.prototype.__defineGetter__( "featPoints", function() {
			if(this.vnum != -1) {
				return 0;
			}
			var rs = sqlQuery("SELECT feat_points FROM users WHERE user_id="+sqlEsc(this.id)+";");
			var featPoints = rs.getRow.get("feat_points");
			return parseInt(featPoints);
		}
	)
	// // Set featPoints
	// JSCharacter.prototype.__defineSetter__( "featPoints", function(p) {
			// if ( !isNumber(p) ) {
				// mudLog(3,103,"Error: Non-number value attempting to be assigned to JSCharacter.featPoints.");
				// return;
			// }
			// if( this.vnum != -1 ) {
				// return;
			// }
			// p = parseInt( Math.ceil(p) );
			// var initial = this.featPoints;
			// var difference = p-initial;
			// if ( difference == 0 ) {
				// return false;
			// }
			// var s = "";
			// if ( Math.abs(difference) > 1 ) {
				// s = "s";
			// }
			// var transaction = "acquired";
			// if ( difference < 0 ) {
				// transaction = "lost";
			// }
			// sqlQuery("UPDATE users SET feat_points="+sqlEsc(parseInt(p))+" WHERE user_id="+sqlEsc(this.id)+";");
			// mudLog(3,103,this.name+" "+transaction+" "+Math.abs(difference)+" feat point"+s+". ("+initial+" to "+p+")");
			// this.send("You have "+transaction+" "+Math.abs(difference)+" feat point"+s+"!");
			// return true;
		// }
	// )
	
	JSCharacter.prototype.addFeatPoints = function( amount ) {
			if ( !isNumber(amount) ) {
				mudLog(3,103,"Error: Non-number value attempting to be assigned to JSCharacter.featPoints.");
				return;
			}
			if( this.vnum != -1 ) {
				return;
			}
			var initial = this.featPoints;
			amount = parseInt( Math.ceil(amount) );
			if ( amount == 0 ) {
				return false;
			}
			var s = "";
			if ( Math.abs(amount) > 1 ) {
				s = "s";
			}
			var transaction = "acquired";
			if ( amount < 0 ) {
				transaction = "lost";
			}
			var newAmount = initial+amount;
			sqlQuery("UPDATE users SET feat_points="+sqlEsc(parseInt(newAmount))+" WHERE user_id="+sqlEsc(this.id)+";");
			mudLog(3,103,this.name+" "+transaction+" "+Math.abs(amount)+" feat point"+s+". ("+initial+" to "+newAmount+")");
			this.send("You have "+transaction+" "+Math.abs(amount)+" feat point"+s+"!");
			return true;
	}
	/**Test by Koradin
	JSCharacter.prototype.addWeavePoints = function( amount ) {
			if ( !isNumber(amount) ) {
				mudLog(3,103,"Error: Non-number value attempting to be assigned to JSCharacter.weavePoints.");
				return;
			}
			if( this.vnum != -1 ) {
				return;
			}
			var initial = this.weavePoints;
			amount = parseInt( Math.ceil(amount) );
			if ( amount == 0 ) {
				return false;
			}
			var s = "";
			if ( Math.abs(amount) > 1 ) {
				s = "s";
			}
			var transaction = "acquired";
			if ( amount < 0 ) {
				transaction = "lost";
			}
			var newAmount = initial+amount;
			sqlQuery("UPDATE users SET weave="+sqlEsc(parseInt(newAmount))+" WHERE user_id="+sqlEsc(this.id)+";");
			mudLog(3,103,this.name+" "+transaction+" "+Math.abs(amount)+" weave point"+s+". ("+initial+" to "+newAmount+")");
			this.send("You have "+transaction+" "+Math.abs(amount)+" weave point"+s+"!");
			return true;
	} **/

