var script189 = function(self, actor, here, args, extra) {
	//Standardized Script for Clan Cloak and Master Weapon turn in
	//Original by Alder, rewritten in JavaScript by Fogel
	var clan = 0;
	var obj = extra.obj;
	for( var x = 0; x < constants.CLANS_MAX; x++ )
	{
		if( self.inClan(x) )
			clan = x;
	}
	waitpulse 2;
	if( obj.canWear( constants.ITEM_WEAR_ABOUT ) && obj.clan )
	{
		self.comm( "award " + actor.name + " " + clan + " " + global.trinketCost + " Clan cloak turned in" );
		actor.comm( "save" );
		obj.extract();
	}
		
	else if( obj.canWear( constants.ITEM_WEAR_WIELD ) && isName( "masterweapon", obj.namelist ) )
	{
		if( obj.vnum == actor.masterWeapon && actor.quest( "master_clan_check" ) == clan )
		{
			self.comm( "award " + actor.name + " " + clan + " " + global.masterWeaponIssue + " Master weapon turned in" );
			actor.comm( "save" );
			waitpulse 2;
			self.comm( "say I will hold onto this for you." );
			waitpulse 4;
			self.comm( "emote places " + obj.name + " inside a chest before closing the lid." );
			obj.extract();
		}
		else if( actor.quest( "master_clan_check" ) != clan && obj.vnum == actor.masterWeapon)
		{
			waitpulse 2;
			self.comm( "say You didn't issue this from me." );
			waitpulse 2;
			self.comm( "drop masterweapon" );
		}
		else if( obj.vnum != actor.masterWeapon )
		{
			self.comm( "award " + actor.name + " " + clan + " " + global.masterWeaponRetrieve + " Master weapon turned in");
			actor.comm( "save" );
			obj.extract();
			waitpulse 2;
			self.comm( "say Very well done " + actor.name + ". You have managed to slay a master and retrieve their weapon." );
		}
	}
	else
	{
		self.comm( "drop all" );
	}
	
}