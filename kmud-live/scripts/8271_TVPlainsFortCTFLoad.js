var script8271 = function(self, actor, here, args, extra)
{
	//Alder
//TV Plains Fort CTF
//1=ds 2=ls -1=no load
var side = getSval(self,8271,"TV_PLAINS_CTF");
// self.echo("Side: "+side);
if ( side == -1 )
	return;
if ( !side || side == 0)
	side = random(1,2);
var leader, follower1, follower2, leadName;
switch ( side ) {
	case 1:
		leader    = 3002;
		follower1 = 8031;
		follower2 = 8031;
		leadName = "half-man";
		break;
	case 2:
		leader    = 19513;
		follower1 = 8271;
		follower2 = 8271;
		leadName = "officer";
		break;
}
self.loadMob(leader);    leader    = self.people[0];
self.loadMob(follower1); follower1 = self.people[0];
self.loadMob(follower2); follower2 = self.people[0];
follower1.comm("follow "+leadName);
follower2.comm("follow "+leadName);
leader.comm("group all");
setSval(self,8271,"TV_PLAINS_CTF",-1);

};

