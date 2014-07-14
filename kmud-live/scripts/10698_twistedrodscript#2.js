var script10698 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
var targetStr = vArgs[1];
if( isName(targetStr, self.namelist) ) {
     wait 1
     getCharCols(actor);
     actor.send(bld+cyn+"The rod feels cool to the touch and soothing, almost as if using it would replenish you..."+nrm);
}

};

