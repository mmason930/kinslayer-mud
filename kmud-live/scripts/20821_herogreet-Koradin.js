var script20821 = function(self, actor, here, args, extra)
{
	//Hero greet
//Koradin - April 2010
if (getSval(self,20820,"talking") == 3)
	return;
if (actor.quest("artifact_global") == 1)
{
	wait 1;
	self.say("Ah, "+actor.name+", good to see you again!  Do you have what I seek?");
	return;
}

};

