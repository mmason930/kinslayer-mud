var script20825 = function(self, actor, here, args, extra)
{
	//Chooses a new herb to buy for higher price at random
//Koradin - May 2010
var new_herb = random(20985,20998);
self.comm("yell I'm looking for "+getObjProto(new_herb).name+"! Come see me and I'll pay you extra!");
setSval(self,20825,"new herb", new_herb);
setSval(self,20825,"herb tries", random(2,6));

};

