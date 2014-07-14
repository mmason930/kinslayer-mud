var script20710 = function(self, actor, here, args, extra)
{
	//animal fat regen
//Koradin - June 2010
act(self.cyan(constants.CL_SPARSE) + "You feel your body converting the animal fat to energy..." + self.normal(constants.CL_SPARSE), true, self, null, null, constants.TO_CHAR);
act("$n moves a bit more quickly, looking more energetic.", true, self, null, null, constants.TO_ROOM);
var decay = getSval(self,20709,"decay");
if (decay == null)
        decay = 0;
decay = (100 - decay);
var heal = Math.floor((self.maxMove / 20) * (decay / 100) );
self.mvs += heal;
setSval(self, 20709, "regen effect", (getSval(self, 20709, "regen effect") + 1))
if (getSval(self, 20709, "regen effect") > 5)
{
		self.send("As the energy wears off, you realize you don't feel so well...");
		act("$n's face pales a bit as $e realizes what $e ate.",true,self,null,null,constants.TO_ROOM);
		var time = 0;
		if (decay < 25)
			time += 1;
		if (decay < 50)
			time += 1;
		if (decay < 75)
			time += 1;
		self.affect(constants.AFF_POISON,time,0,0);
        setSval(self, 20709, "regen effect", 0);
        setSval(self, 20709, "decay", 0);
        self.detach(20710);
        return;
}

};

