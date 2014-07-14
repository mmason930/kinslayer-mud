var script10660 = function(self, actor, here, args, extra)
{
	//Jenkin 
//October 2010
//Quest: Eternal Servitude
_block;
var arg_array = args.split(" ");
if( arg_array.length != 2 )
{
        actor.send("Syntax is: recruit [victim]");
        return;
}
else if( arg_array.length >= 2 ) {
var target = actor.getCharRoomVis(arg_array[1]);
if (target == undefined ) {
        actor.send("They don't seem to be here.");
        return;
}
var qName = 'Eternal Servitude';
var recruits = getSval(actor, 10660, "greyman_recruit");
if( !recruits ) { 
   recruits = 0;
}
var leader = getSval(target, 10660, "leader");   
if( actor == leader) { 
        actor.send("They're already following you!");
        return; 
}         
if( !target )  {
        actor.send("Recruit who?");
        return;
}
if( target.vnum != 6478 ) {
       waitpulse 5;
       target.say("I'll die before I serve you!");
       waitpulse 5;
       target.comm("k " + actor.name);
       return;
}
   if ( recruits < 4) {
     target.comm("grovel " + actor.name);
     waitpulse 7;
     target.say("I live to serve, Master.");
     waitpulse 5;
     target.comm("follow " + actor.name);
     var message = "Your private army is now slightly larger.";     
     recruits += 1;
     setSval(actor, 10660, "greyman_recruit", recruits);   
     setSval(target, 10660, "leader", actor);   
     }
   else {
     target.comm("grovel " + actor.name);
     waitpulse 7;
     target.say("I live to serve, Master.");
     waitpulse 5;
     target.comm("follow " + actor.name);
     var message = "You have gathered enough greymen. Return north.";
     actor.updateJournalTask(qName, 1);
     actor.detach(10660);
   }
   getCharCols(actor);
   actor.send(bld+cyn+ message +nrm); 
 }

};

