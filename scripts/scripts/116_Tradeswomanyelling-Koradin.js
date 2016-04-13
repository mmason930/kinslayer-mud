var script116 = function(self, actor, here, args, extra) {
	var tArr = ["Smithing","Mining","Herbalism","Skinning","Tailoring","Mixing"];
	var pArr = ["Blacksmith","Miner","Herbalist","Carver","Tailor","Alchemist"];
	if (random(1,2) == 1)
		self.comm("yell Think you've got what it takes to become an Artisan "+pArr[random(0,5)]+"? Come see me at "+self.room.name+"!");
	else
		self.comm("yell Want to learn more about "+tArr[random(0,5)]+"? I'm sharing stories at "+self.room.name+"!");
}