var script22305 = function(self, actor, here, args, extra)
{
	waitpulse 1;
if( actor.clanCheck( [16,17,18,19,20,21,22,23] ) == false && actor.class == constants.CLASS_CHANNELER
&& actor.sex == constants.SEX_FEMALE && actor.level > 24 )
{
	wait 2;
	if( actor ) {
		self.say( actor.name + ", you seem to have the ability to channel. Will you put your name to the Book of Novices?" );
		actor.qval( 'novice_proposition', 1 );
	}
}

};

