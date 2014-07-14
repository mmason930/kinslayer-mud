var script202 = function(self, actor, here, args, extra)
{
	var ch = self;
if( !getSval( ch, 202, "spawnRoom" ) )
{
	setSval( ch, 202, "spawnRoom", ch.room.vnum );
	setSval( ch, 202, "mountlessTime", 1 );
	return;
}
var spawn = getRoom( getSval( ch, 202, "spawnRoom" ) );
if( ch.room == spawn || ch.fighting )
{
	return;
}
else if( ch.riddenBy )
{
	setSval( ch, 202, "mountlessTime", 1 );
	return;
}
else
{
	setSval( ch, 202, "mountlessTime", getSval( ch, 202, "mountlessTime" ) + 1 );
}
if( getSval( ch, 202, "mountlessTime" ) > 60 )
{
	ch.room.echo( capFirstLetter(ch.name) + " gets restless and gallops home." );
	ch.moveToRoom( spawn );
	ch.room.echo( capFirstLetter(ch.name) + " wanders in, tired from a long run." );
	ch.mvs = ch.maxMove / 2;
	setSval( ch, 202, "mountlessTime", 1 );
}

};

