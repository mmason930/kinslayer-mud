var script922 = function(self, actor, here, args, extra)
{
	//Alder
//June 2011
/** Mashadar Kills Victims **/
if ( !self.victims.length ) {
	return;
}
var messages = [];
messages.push(["$n's legs begin to tremble violently before ceasing abruptly and exploding all over you in a bloody and bone-riddled mess.","Your legs begin to tremble violently before ceasing abruptly and exploding in a bloody, bone-riddled mess."]);
messages.push(["$n is thrown to the ground by an invisible force and $s body begins contorting until a loud crunching sound is heard as every bone in $s is destroyed.","You are thrown to the ground by an invisible force and your body begins contorting until a loud crunching sound is heard as every bone in your body is destroyed."]);
messages.push(["Blood starts to drain from all of $n's orifices as the stream intensifies to a jet of red and $n crumples into a red pool containing all of $s body's blood.","Blood starts to drain from all of your orifices as the stream intensifies to a jet of red and you crumples into a red pool containing all of your body's blood."]);
while ( self.victims.length ) {
	var victim = self.victims.shift();
	var selection = messages[random(0,2)];
	act(selection[0],true,victim,null,null,constants.TO_ROOM);
	victim.send(selection[1]);
	victim.damage(victim.hps+10);
	waitpulse 7;
}
self.comm(dirToText(here.getRandomExitDirection()));

};

