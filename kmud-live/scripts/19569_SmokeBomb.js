var script19569 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
var targetStr = vArgs[1];
if( isName(targetStr, self.namelist) ) {
  waitpulse 4;
  actor.send("You vanish in a cloud of smoke!");
  actor.moveToRoom(getRoom(1700));
  actor.comm("look");
  self.extract();
}

};

