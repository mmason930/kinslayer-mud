/**************************************************************************
 *                                                                        *
 * JSCharacter.setOLC( o )                                                *
 * Arguments: o - The object that the user's OLC will be set to           *
 * Purpose: Sets the user's OLC structure                                 *
 *                                                                        *
 * ~~~ By: Galnor 01/22/2010                                              *
 *                                                                        *
 **************************************************************************/
JSCharacter.prototype.setOLC =
function( o )
{
	setSval(this, -13275, "olc", o);
}
/**************************************************************************
 *                                                                        *
 * JSCharacter.getOLC()                                                   *
 * Arguments: <None>                                                      *
 * Purpose: Obtain user's OLC structure                                   *
 *                                                                        *
 * ~~~ By: Galnor 01/22/2010                                              *
 *                                                                        *
 **************************************************************************/
JSCharacter.prototype.getOLC =
function()
{
	return getSval(this, -13275, "olc");
}
