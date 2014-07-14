var script10687 = function(self, actor, here, args, extra)
{
	//      Allows a clanned human, fade or dreadlord to issue a horse
//      This is a hack to give Whitecloaks horse support.
if( getSval( self, 207, "issuing" ) == 1 )
        return;
setSval( self, 207, "issuing", 1 );
var argv = args.split(" ");
var horse = getSval( actor, 207, "horse" );
if( horse )
{
        waitpulse 2;
        self.comm("say You already have one issued!");
}
else if( actor.race == constants.RACE_TROLLOC && !( actor.class == constants.CLASS_FADE || actor.class == constants.CLASS_DREADLORD) )
{
        waitpulse 2;
        self.comm("say I don't serve your kind here!");
}
else if( actor.affectedBy(constants.AFF_NOQUIT) )
{
        waitpulse 2;
        self.comm("say Calm down first, you'll spook the horses!");
}
else if( argv[1] == "horse" && !horse && actor.name == "Hayato" || actor.name == "Jule" || actor.name == "Robby" ||actor.name == "Alec" || actor.name == "Kazuya" || actor.name == "Cain" || actor.name == "Jenkin" || actor.name == "Robby" )
{
        horse = 201;
        waitpulse 4;
        self.comm("say Right away, " + actor.name + "!");
        waitpulse 4;
        here.loadMob(horse);
        horse = here.people[0];
        echoaround( actor, self.name + " leads " + horse.name + " to " + actor.name + ".");
        actor.send( self.name + " leads " + horse.name + " up to you." );
        waitpulse 4;
        horse.comm("follow " + actor.name);
}
setSval( self, 207, "issuing", 0 );

};

