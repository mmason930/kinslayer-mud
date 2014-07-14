var script7 = function(self, actor, here, args, extra)
{
	var weapon = actor.eq(constants.WEAR_WIELD)
if( self.vnum == -1 && actor.vnum == -1 )
{
	if ( weapon == null )
	{
		return;
	}
	if ( weapon.weaponType != constants.WEAPON_CLUB )
	{
		return;
	}
	if( self.race != actor.race && self.level >= 30 )
		{
			actor.updateJournalTask( "Ghob'hlin Advancement 3", 0 )
		}
};

};

