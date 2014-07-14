var script604 = function(self, actor, here, args, extra)
{
	//Standardized Script for Clan Cloak and Master Weapon issuing
//Original by Alder, rewritten in JavaScript by Fogel
var argv = args.split(" ");
var clan = 0;
for( var x = 0; x < constants.CLANS_MAX; x++ )
{
	if( self.inClan(x) )
		clan = x;
}
if( !actor.inClan(clan) )
{
	self.comm( "say We don't serve your kind here!" );
}
else if( argv[1] == "master" || argv[1] == "masterweapon" )
{
	if( actor.masterWeapon && actor.qp(clan) >= global.masterWeaponIssue && ( getObjProto(actor.masterWeapon) ).count < 1)
	{
		waitpulse 4;
		self.comm( "say One Moment..." );
		waitpulse 2;
		self.comm( "emote opens a chest and hands you an item from inside." );
		self.loadObj( actor.masterWeapon );
		waitpulse 4;
		self.comm( "give masterweapon " + actor.name );
		self.comm( "drop masterweapon ");
		self.comm( "award " + actor.name + " " + clan + " " + (-1 * global.masterWeaponIssue) + " Master weapon issued");
		actor.qval( "master_clan_check", clan );
		actor.comm( "save" );
		waitpulse 4;
		self.comm( "say Use it well." );
	}
	else if( actor.masterWeapon && actor.qp(clan) < global.masterWeaponIssue )
	{
		waitpulse 4;
		self.comm( "say You don't have enough quest points." );
	}
	else if( ( getObjProto(actor.masterWeapon) ).count >= 1 )
	{
		waitpulse 4;
		self.comm( "say You have already issued a master weapon!" );
	}
	else
	{
		waitpulse 4;
		self.comm( "say But you have not acquired one yet." );
	}
}
else if( argv[1] == "cloak" )
{
	if( actor.qp(clan) >= global.trinketCost )
	{
		waitpulse 4;
		self.comm( "say Try not to lose this one." );
		waitpulse 4;
		self.loadObj( global.clanCloaks[clan] );
		self.comm( "give clancloak " + actor.name );
		self.comm( "drop clancloak ");
		self.comm( "award " + actor.name + " " + clan + " " + (-1 * global.trinketCost) + " Clan cloak issued" );
		actor.comm( "save" );
	}
	else
	{
		waitpulse 4;
		self.comm( "say You don't have enough quest points." );
	}
}



};

