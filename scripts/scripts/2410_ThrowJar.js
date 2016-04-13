var script2410 = function(self, actor, here, args, extra) {
	var arg_list = getArgList(args);
	if( arg_list.length <= 1 )
	{
	    _noblock;
		return;
	}
	if( isName(arg_list[1], self.namelist) )
	{
	    _block;
		actor.send("You lift " + self.name + " over your head.");
		here.echoaround(actor, actor.name + " lifts " + self.name + " over " + actor.hisher + " head.");
		actor.startTimer(3);
		var success = runTimer(actor);
		
		if( success )
		{
		    actor.send("You toss " + self.name + ".");
			here.echoaround(actor, actor.name + " tosses " + self.name + ".");
			self.moveToRoom(getRoom(2));
			waitpulse 8;
			here.echo(capFirstLetter(self.name) + " shatters as it hits the ground.");
			self.extract();
		}
	}
	else
	    _noblock;
	/****
	   if (!%(arg0%))
	   return 0
	   halt
	   end
	   set obj %(actor.carrying%)
	   while (%(obj%))
	   if (%(IsName(%(arg0%),%(obj.name%))%))
	   break
	   end
	   set obj %(obj.next_content%)
	   done
	   if (!%(obj%))
	   return 0
	   halt
	   end
	   %(send%) %(actor%) You lift %(obj.shortdesc%) and toss it.
	   %(echoaround%) %(actor%) %(actor.name%) lifts %(obj.shortdesc%) and tosses it.
	   wait %(Random(8,12)%)
	   %(echo%) %(self.shortdesc%) shatters into pieces!
	   obj_from_char %(obj%)
	   obj_to_room %(obj%) %(actor.room.vnum%)
	   if (%(Random(1,100)%) <= 4)
	   oload o 3189
	   end
	   odamage %(actor%) 2 m
	   opurge self
	   ****/
}