/**************************************************************************
 *                                                                        *
 * JSCharacter.setupOLC()                                                 *
 * Arguments: type - The OLC type(pedit,redit,oedit,etc)                  *
 * Purpose: Initiates user's OLC data structure                           *
 *                                                                        *
 * ~~~ By: Galnor 01/22/2010                                              *
 *                                                                        *
 **************************************************************************/
JSCharacter.prototype.setupOLC =
function( type )
{
	var olc = new Object();
	olc.type = type;
	olc.mode = "MODE_MAIN";
	olc.owner = this;
	olc.getModeObject = function() {
		for(var i = 0;i < vOLC.length;++i) {
			if( vOLC[i].type == this.type ) {
				for(var m = 0;m < vOLC[i].modes.length;++m) {
					if( vOLC[i].modes[m].mode == this.mode )
						return vOLC[i].modes[m];
				}
			}
		}
	}
	olc.switchToMode = function( mode ) {
		this.mode = mode;
		if( mode == undefined ) {
			mudLog(constants.NRM, 102, "No mode specified in OLC.switchToMode()");
		}
		var mObject = this.getModeObject();
		if( mObject.display ) {
			getCharCols( this.owner );
			mObject.display( this.owner );
		}
	}
	this.setOLC( olc );
	this.attach( 13274 );
	
	if( !olc.getModeObject() ) {
		actor.send("Error loading OLC : olc.getModeObject() not found.");
		actor.cleanupOLC();
		return;
	}
	
	act( "$n begins using OLC.", true, this, null, null, constants.TO_ROOM );
	mudLog( constants.CL_COMPLETE, Math.max(102,this.invis), this.name + " begins using " + olc.type + "." );
	getCharCols(this,constants.CL_COMPLETE);
	olc.getModeObject().display(this);
}
/**************************************************************************
 *                                                                        *
 * JSCharacter.cleanupOLC()                                               *
 * Arguments: <None>                                                      *
 * Purpose: Destroys the user's OLC data structure                        *
 *                                                                        *
 * ~~~ By: Galnor 01/22/2010                                              *
 *                                                                        *
 **************************************************************************/
JSCharacter.prototype.cleanupOLC =
function( type )
{
	mudLog( constants.CL_COMPLETE, Math.max(102,this.invis), this.name + " stops using " + this.getOLC().type + "." );
	this.setOLC( undefined );
	this.detach( 13274 );
	act( "$n stops using OLC.", true, this, null, null, constants.TO_ROOM );
}

