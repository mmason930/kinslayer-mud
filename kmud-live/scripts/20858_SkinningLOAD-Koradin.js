var script20858 = function(self, actor, here, args, extra)
{
	//Skinning load
//Koradin - May 2010
var afterWait = function()
{
var actor = self.carriedBy;
getCharCols(actor);
if (random(1,100) < 4)
{
actor.send("You find "+cyn+"a chunk of animal fat"+nrm+" attached to the "+actor.inventory[0].namelist.split(" ")[1]+"!");
actor.loadObj(4905);
}
var skill = actor.getSkill(getSkillVnum("Skinning"));
if (self.vnum == 4900)//SKIN
{
var mat = "skin";
if (skill > 39 && random(skill,100) == skill)
var size = [4927,"large"];
else if (skill > 19 && random(1,2) == 1)
var size = [4926,"medium"];
else
var size = [4925,"small"];
}
else if (self.vnum == 4901)//HIDE
{
var mat = "hide";
if (skill > 59 && random(skill,100) == skill)
var size = [4930,"large"];
else if (skill > 39 && random(1,2) == 1)
var size = [4929,"medium"];
else
var size = [4928,"small"];
}
else if (self.vnum == 4902)//FUR
{
var mat = "fur";
if (skill > 98 && random(skill,100) == skill)
var size = [4933,"large"];
else if (skill > 89 && random(1,2) == 1)
var size = [4932,"medium"];
else
var size = [4931,"small"];
}
else if (self.vnum == 4903)//PELT
{
var mat = "pelt";
if (skill > 79 && random(skill,100) == skill)
var size = [4936,"large"];
else if (skill > 49 && random(1,2) == 1)
var size = [4935,"medium"];
else
var size = [4934,"small"];
}
else if (self.vnum == 4904)//SCALES
{
var mat = "scales";
if (skill > 94 && random(skill,100) == skill)
var size = [4939,"large"];
else if (skill > 84 && random(1,2) == 1)
var size = [4938,"medium"];
else
var size = [4937,"small"];
}
actor.loadObj(size[0]);
awardPracs(actor,"Skinning",mat,size[1]);
self.extract();
}
setTimeout(1,afterWait,[self]);

};

