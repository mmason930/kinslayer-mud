/**************************************************************************
 *                                                                        *
 * getCharCols( actor, pref )                                             *
 * Arguments: actor - The player whose colors we are getting              *
 *            pref  - The player's color preferences. CL_NORMAL by default*
 * Purpose: To retrieve a player's colors based on preference             *
 *                                                                        *
 * ~~~ By: Galnor 01/22/2010                                              *
 *                                                                        *
 **************************************************************************/
if( !yel )	var yel;
if( !cyn )	var cyn;
if( !red )	var red;
if( !mag )	var mag;
if( !blu )	var blu;
if( !grn )	var grn;
//if( !whi )	var whi;
if( !bld )	var bld;
if( !nrm )	var nrm;
 function getCharCols( actor, pref ) {
	if( !pref )
		pref = constants.CL_NORMAL;
	yel = actor.yellow( pref );
	cyn = actor.cyan( pref );
	red = actor.red( pref );
	mag = actor.magenta( pref );
	blu = actor.blue( pref );
	grn = actor.green( pref );
//	whi = actor.white( pref );
	bld = actor.bold( pref );
	nrm = actor.normal( pref );
}
 
