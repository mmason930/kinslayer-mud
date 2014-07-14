var script1121 = function(self, actor, here, args, extra)
{
	var tormRoom = 10256;
var tormVnum = 10256;
var torm = getMobAtRoom(10256, 10256);
if(torm && random(0, 1))
{
	waitpulse random(3, 7);
	here.echo( self.name + " lets out a strange, high pitched whistle." );
	wait 1;
	var path = torm.room.pathToRoom( self.room );
	for each( var dir in path )
    {
        if( torm.fighting || torm.room.doorIsClosed(dir) )
            break;
        waitpulse 2;
        torm.comm( dirToText(dir) );
    }
}

};

