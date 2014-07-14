var script1018 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
var obj = self;
var ch = self.wornBy;
if( vArgs[1] == "orb" )
if (actor.sps > 1){
{
	_block;
	if (obj.getPval("wellcharge") == null){
		obj.setPval("wellcharge", 4, true);
	}
	if (obj.getPval("wellcharge") == "4"){
		echoaround(ch, "A glowing green orb held by "+ ch.name +" flashes and fades to a dark black.");
		actor.send("The glowing green orb flashes and fades to a dark black.");
		self.setRetoolDesc("A dark black orb lies here.");
		self.setRetoolSDesc("a dark black orb");
		self.setRetoolName("dark black orb");
		if (actor.sps + 81 > actor.maxSpell){
			actor.send("You draw too much of the One Power from the orb!")
			actor.hps -= 50 + random(25, 50);
			actor.sps -=150;
		}
		else{
			actor.send("You draw some of the One Power from the orb.");
			actor.sps += 81; 
		}
		obj.setPval("wellcharge", 1, true);
		waitpulse 1300;
		obj.setPval("wellcharge", 2, true);
		self.setRetoolDesc("A pale green orb lies here.");
		self.setRetoolSDesc("a pale green orb");
		self.setRetoolName("pale green orb");
		if (obj == actor.eq(constants.WEAR_HOLD)){
			echoaround(obj.wornBy, "A dark black orb held by "+ obj.wornBy.name +" begins to glow a pale green.");
			obj.wornBy.send("The dark black orb in your hands begins to glow a pale green.");
		}
		waitpulse 1300;
		if (obj.getPval("wellcharge") == "2"){
			obj.setPval("wellcharge", 3, true);
			self.setRetoolDesc("A green orb lies here.");
			self.setRetoolSDesc("a green orb");
			self.setRetoolName("green orb");
			if (obj == actor.eq(constants.WEAR_HOLD)){
				echoaround(obj.wornBy, "A pale green orb held by "+ obj.wornBy.name +" begins to glow green.");
				obj.wornBy.send("The pale green orb in your hands begins to glow green.");
			}
		}
		else{
			return;
		}
		waitpulse 1300;
		if (obj.getPval("wellcharge") == "3"){ 
			obj.setPval("wellcharge", 4, true);
			self.setRetoolDesc("A bright green orb lies here.");
			self.setRetoolSDesc("a bright green orb");
			self.setRetoolName("bright green orb");
			if (obj == actor.eq(constants.WEAR_HOLD)){
				echoaround(obj.wornBy, "A green orb held by "+ obj.wornBy.name +" begins to glow bright green.");
				obj.wornBy.send("The green orb in your hands begins to glow a bright green.");
						}
		}
		else{
			return;
		}
	
	}
	}
if (obj.getPval("wellcharge") == "3"){
	echoaround(ch, "A green orb held by "+ ch.name +" flashes and fades to a dark black.");
	actor.send("The green orb flashes and fades to a dark black.");
	self.setRetoolDesc("A dark black orb lies here.");
	self.setRetoolSDesc("a dark black orb");
	self.setRetoolName("dark black orb");
	if (actor.sps + 54 > actor.maxSpell){
		actor.send("You draw too much of the One Power from the orb!")
		actor.hps -= 50 + random(25, 50);
		actor.sps -=150;
	}
	else{
		actor.send("You draw some of the One Power from the orb.")
		actor.sps += 54; 
	}
	obj.setPval("wellcharge", 1, true);
	waitpulse 1300;
	obj.setPval("wellcharge", 2, true);
	self.setRetoolDesc("A pale green orb lies here.");
	self.setRetoolSDesc("a pale green orb");
	self.setRetoolName("pale green orb");
	if (obj == actor.eq(constants.WEAR_HOLD)){
		echoaround(obj.wornBy, "A dark black orb held by "+ obj.wornBy.name +" begins to glow a pale green.");
		obj.wornBy.send("The dark black orb in your hands begins to glow a pale green.");
	}
	waitpulse 1300;
	if (obj.getPval("wellcharge") == "2"){
		obj.setPval("wellcharge", 3, true);
		self.setRetoolDesc("A green orb lies here.");
		self.setRetoolSDesc("a green orb");
		self.setRetoolName("green orb");
		if (obj == actor.eq(constants.WEAR_HOLD)){
			echoaround(obj.wornBy, "A pale orb held by "+ obj.wornBy.name +" begins to glow green.");
			obj.wornBy.send("The pale green orb in your hands begins to glow green.");
		}
	}
	else{
		return;
	}
	waitpulse 1300;
	if (obj.getPval("wellcharge") == "3"){
		obj.setPval("wellcharge", 4, true);
		self.setRetoolDesc("A bright green orb lies here.");
		self.setRetoolSDesc("a bright green orb");
		self.setRetoolName("bright green orb");
		if (obj == actor.eq(constants.WEAR_HOLD)){
			echoaround(obj.wornBy, "A green orb held by "+ obj.wornBy.name +" begins to glow bright green.");
			obj.wornBy.send("The green orb in your hands begins to glow a bright green.");
		}
	}
	else{
		return;
	}
}
if (obj.getPval("wellcharge") == "2"){
	echoaround(ch, "A pale green orb held by "+ ch.name +" flashes and fades to a dark black.");
	actor.send("The pale green orb flashes and fades to a dark black.");
	self.setRetoolDesc("A dark black orb lies here.");
	self.setRetoolSDesc("a dark black orb");
	self.setRetoolName("dark black orb");
	if (actor.sps + 26 > actor.maxSpell){
		actor.send("You draw too much of the One Power from the orb!")
		actor.hps -= 50 + random(25, 50);
		actor.sps -=150;
	}
	else{
		actor.send("You draw some of the One Power from the orb.")
		actor.sps += 26; 
	}
	obj.setPval("wellcharge", 1, true);
	waitpulse 1300;
	obj.setPval("wellcharge", 2, true);
	if (obj == actor.eq(constants.WEAR_HOLD)){
		echoaround(obj.wornBy, "A dark black orb held by "+ obj.wornBy.name +" begins to glow a pale green.");
		obj.wornBy.send("The dark black orb in your hands begins to glow a pale green.");
	}
	if (obj.getPval("wellcharge") == "2"){
		obj.setPval("wellcharge", 3, true);
		self.setRetoolDesc("A green orb lies here.");
		self.setRetoolSDesc("a green orb");
		self.setRetoolName("green orb");
		if (obj == actor.eq(constants.WEAR_HOLD)){
			echoaround(obj.wornBy, "A pale green orb held by "+ obj.wornBy.name +" begins to glow green.");
			obj.wornBy.send("The pale green orb in your hands begins to glow green.");
		}
	}
	else{
		return;
	}
	waitpulse 1300;
	if (obj.getPval("wellcharge") == "3"){ 
		obj.setPval("wellcharge", 4, true);
		self.setRetoolDesc("A bright green orb lies here.");
		self.setRetoolSDesc("a bright green orb");
		self.setRetoolName("bright green orb");
		if (obj == actor.eq(constants.WEAR_HOLD)){
			echoaround(obj.wornBy, "A green orb held by "+ obj.wornBy.name +" begins to glow bright green.");
			obj.wornBy.send("The green orb in your hands begins to glow a bright green.");
		}
	}
	else{
		return;
	}
}
if (obj.getPval("wellcharge") == "1"){
	actor.send("You try to draw from the orb, but come away with nothing.");
	waitpulse 5000;
	if (obj.getPval("wellcharge") == "1"){
		obj.setPval("wellcharge", 4, true);
		if (obj == actor.eq(constants.WEAR_HOLD)){
			echoaround(obj.wornBy, "A dark black orb held by "+ obj.wornBy.name +" begins to glow a bright green.");
			obj.wornBy.send("The dark black orb in your hands begins to glow a bright green.");
			}
		self.setRetoolDesc("A bright green orb lies here.");
		self.setRetoolSDesc("a bright green orb");
		self.setRetoolName("bright green orb");
		}
}
}


};

