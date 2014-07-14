var script195 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
var clan = self.findClans();
if( !actor.inClan(clan) || (actor.getRank(clan) < 6 && !actor.clanCheck(25)) )
	return;
else if( strn_cmp(vArgs[0], "show", 3) && strn_cmp(vArgs[1], "skills", 3) )
{
	wait 1;
	self.say( actor.name + ", these are the skills available for you to learn: " );
	wait 2;
	self.say( "'Dual Bash' - allows you to bash a second victim. The second victim must be engaged on you during the duration of the bash timer to be hit." );
	wait 1;
	self.say( "'Switch Target' - allows you to switch between targets who are engaged on you by typing 'kill <victim>'." );
	wait 1;
	self.say( "'Choke' - allows you to 'choke' a victim, dealing damage and preventing them from channeling for a short period of time." );
	wait 2;
	self.say( "When you are ready to learn a new skill, say 'learn <skill name>'. But beware, if you already know one of these skills, it will be forgotten upon learning a new one." )
}
else if( strn_cmp(vArgs[0], "learn", 4) )
{
	var skillNum;
	wait 1;
	if( strn_cmp(vArgs[1], "Dual", 3) && strn_cmp(vArgs[2], "Bash", 3) )
		skillNum = 185;
	else if( strn_cmp(vArgs[1], "Switch", 5) && strn_cmp(vArgs[2], "Target", 4) )
		skillNum = 186;
	else if( strn_cmp(vArgs[1], "Choke", 4) )
		skillNum = 187;
	else
		return;
	actor.setSkill(185, 0);
	actor.setSkill(186, 0);
	actor.setSkill(187, 0);
	actor.setSkill(skillNum, 100);
	self.say("Done.");
}

};

