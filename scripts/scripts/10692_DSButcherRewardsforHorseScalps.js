var script10692 = function(self, actor, here, args, extra) {
	var item = extra.obj;
	args = item.name.split(" ");
	var paidToActor = 0;
	if( args[2] == "scalp" && arrContains(args, "stallion") || arrContains(args, "razor") || arrContains(args, "mare") || arrContains(args, "mule") || arrContains(args, "warhorse")) { 
	  if( arrContains(args, "warhorse") ) {
	      paidToActor = 100; //Black warhorses are so hard to kill they deserve 1 silver for the deed
	  }
	  if( arrContains(args, "stallion") || arrContains(args, "razor") || arrContains(args, "mare") || arrContains(args, "mule")) {
	      paidToActor = 10; 
	  }       
	waitpulse 4;
	self.comm("lick");
	waitpulse 4;
	self.comm("say Luks gud enough ta eet!");
	actor.send(capFirstLetter(self.name) +" fumbles in his purse, and hands you" +actor.numCopperToText(paidToActor,true) + ".");
	act(capFirstLetter(self.name) + " hands some coins to $n.",true,actor,null,null,constants.TO_ROOM);
	actor.gold += paidToActor;
	item.extract();
	}
	else {
	  self.say("I dun wan this!");
	  waitpulse 5;
	  self.comm("drop");
	}
	
}