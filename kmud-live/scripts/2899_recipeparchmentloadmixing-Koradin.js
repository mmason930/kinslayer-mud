var script2899 = function(self, actor, here, args, extra)
{
	var typeRoll = random(0,2);
var skillArr = [0,0,0];
for each (var person in self.room.people) {
	//sendKoradin(person.name)
	var skillLevelArr = [ person.getSkill(getSkillVnum("Smithing")), person.getSkill(getSkillVnum("Tailoring")), person.getSkill(getSkillVnum("Mixing")) ];
	for (var i = 0; i < skillLevelArr.length; i++) {
		if (skillLevelArr[i] > 0) {
			skillArr[i] += 1;
			//sendKoradin("updating for skill "+skillLevelArr[i]);
		}
	}
}
//sendKoradin(skillArr);
var finalArr = [];
var maxSkill = 0;
for (var i = 0; i < skillArr.length; i++) {
	if (skillArr[i] > maxSkill) {
		finalArr = [];
		finalArr.push(i);
		maxSkill = skillArr[i];
	}
	else if (skillArr[i] == maxSkill) {
		finalArr.push(i);
	}
}
if (finalArr.length > 0)
	typeRoll = finalArr[random(0,finalArr.length-1)];
//sendKoradin("final array: "+finalArr);
//sendKoradin("roll: "+typeRoll);
if (typeRoll == 0)
	self.setPval("recType","S",true); // Load a tailoring recipe
else if (typeRoll == 1)
	self.setPval("recType","T",true); // Load a smithing recipe
else
	self.setPval("recType","M",true); // Load a mixing recipe
var rType = self.getPval("recType");
if (rType == "M") {
	var r1 = [1, [109,119,124,129]];
	var r2 = [3, [108,118,123,128]];
	var r3 = [6, [107,117,122,127]];
	var r4 = [10,[106,116,121,126]];
	var r5 = [15,[105,115,121,126]];
	var r6 = [20,[193,198,199,120,125,104,114,103,113,102,112,101,111,100,110,192,191,196,194,195,197,190]];
	var rTotal = [r1,r2,r3,r4,r5,r6];
	var alias = "mixing";
}
else if (rType == "S") {
	var r1 = [5, [269,259,249,239,229,219,209,199,189,179,169,159,149,139,129,119,109,99,89,79,69,59,49,39,29,19]];
	var r2 = [15,[268,258,248,238,228,218,208,198,188,178,168,158,148,138,128,118,108,98,88,78,68,58,48,38,28,18]];
	var r3 = [20,[parseInt(random(1,26).toString() + random(0,7).toString())]];
	var rTotal = [r1,r2,r3];
	var alias = "smithing";
}
else if (rType == "T") {
	var r1 = [5,  [87,95,103,111,119,127,135,143,151,159,167,175,183,191,199,207,215,223]];
	var r2 = [15, [86,94,102,110,118,126,134,142,150,158,166,174,182,190,198,206,214,222]];
	var r3 = [20, [parseInt(random(8,21).toString() + random(0,5).toString())]];
	var rTotal = [r1,r2,r3];
	var alias = "tailoring";
}
else
	return;
var rRoll = random(0,19);
for (i=0;i<rTotal.length;i++)
{
	if (rRoll < rTotal[i][0])
	{
		var recVnum = rTotal[i][1][random(0,rTotal[i][1].length-1)];
		break;
	}
}
var recName = getRecipeByVnum(recVnum, rType).name;
//sendKoradin(recName);
var rName = [];
recName = recName.split(" ");
rName.push(recName[recName.length-1]);
rName.push(recName[recName.length-2]);
rName.push(recName[recName.length-3]);
rName = rName.join(" ");
if (self.vnum == 2899)
{
	self.setPval("recVnum",recVnum,true);
	var sideSelect = random(0,3);
	if (sideSelect == 0)
		self.setPval("exaView","tL",true);
	else if (sideSelect == 1)
		self.setPval("exaView","tR",true);
	else if (sideSelect == 2)
		self.setPval("exaView","bL",true);
	else if (sideSelect == 3)
		self.setPval("exaView","bR",true);
	self.setRetoolName(rName+" "+self.namelist+" "+alias);
}
else if (self.vnum == 2898)
{
	waitpulse 2;
	if (!self.getPval("recVnum"))
		self.setPval("recVnum",recVnum,true);
	if (!self.getPval("exaView"))
	{
		var sideSelect = random(0,5);
		if (sideSelect == 0)
			self.setPval("exaView","tL~tR",true);
		else if (sideSelect == 1)
			self.setPval("exaView","bL~tR",true);
		else if (sideSelect == 2)
			self.setPval("exaView","bR~tR",true);
		else if (sideSelect == 3)
			self.setPval("exaView","bR~tL",true);
		else if (sideSelect == 4)
			self.setPval("exaView","bL~tL",true);
		else if (sideSelect == 5)
			self.setPval("exaView","bL~bR",true);
	}
	self.setRetoolName(rName+" "+self.namelist+" "+alias);
}
else if (self.vnum == 2897)
{
	waitpulse 2;
	if (!self.getPval("recVnum"))
		self.setPval("recVnum",recVnum,true);
	if (!self.getPval("exaView"))
	{
		var sideSelect = random(0,3);
		if (sideSelect == 0)
			self.setPval("exaView","bL~tL~tR",true);
		else if (sideSelect == 1)
			self.setPval("exaView","bL~bR~tR",true);
		else if (sideSelect == 2)
			self.setPval("exaView","bR~tL~tR",true);
		else if (sideSelect == 3)
			self.setPval("exaView","bL~bR~tL",true);
	}
	self.setRetoolName(rName+" "+self.namelist+" "+alias);
}
else if (self.vnum == 2896)
{
	waitpulse 2;
	if (!self.getPval("recVnum"))
		self.setPval("recVnum",recVnum,true);
	if (!self.getPval("exaView"))
		self.setPval("exaView","bL~bR~tL~tR",true);
	self.setRetoolName(rName+" "+self.namelist+" "+alias);
}










};

