var script20884 = function(self, actor, here, args, extra)
{
	//Bar storage for smithing
//Koradin - June 2011
var item = extra.obj;
var afterWait = function() {
	if (self.race != actor.race) {
		self.say("I don't trade with your kind!");
		self.comm("drop "+item.namelist.split(" ")[0]);
		return;
	}
	var skill = actor.getSkill(getSkillVnum("Smithing"));
	if (skill < 40)
		var numStored = 10;
	else if (skill < 75)
		var numStored = 30;
	else if (skill < 99)
		var numStored = 50;
	else
		var numStored = 99;
	if (item.vnum > 4535 && item.vnum < 4583 && (isName("bar",item.name) || isName("brick",item.name))) {
		if (isName("brick",item.name))
			var price = item.cost/10;
		else
			var price = item.cost;
		var storageList = actor.getPval("barStorage");
		//here.echo("current PVAL: " +storageList); 
		if (!storageList) {
			//here.echo("new PVAL: "+item.vnum+","+1);
			self.say("That'll be"+actor.numCopperToText(price, true)+" when you want it back!");
			act("$n opens a locker and tosses "+item.name+" into it.",true,self,null,null,constants.TO_ROOM);
			actor.setPval("barStorage",item.vnum+","+1,true);
			item.moveToRoom(getRoom(20899));
			item.extract();
			return;
		}
		else {
			storageList = storageList.split("~");
			if (storageList.length == 3) {
				var different = true;
				for each (var sItem in storageList) {
					sItem = sItem.split(",");
					if (parseInt(sItem[0]) == item.vnum)
						different = false;
					//sendKoradin(sItem+" ~ "+different);
				}
			}
			if (different == true) {
				self.say("Sorry, you can only store three different types at a time.");
				self.comm("give "+item.namelist.split(" ")[0]+" "+actor.name);
				return;
			}
			var storArr = [];
			var itemVnum = null;
			var itemCount = null;
			for (i=0;i<storageList.length;i++) {
				storageList[i] = storageList[i].split(",");
				itemVnum = parseInt(storageList[i][0]);
				itemCount = parseInt(storageList[i][1]);
				storArr.push([itemVnum,itemCount]);
			}
			var totalBars = 0;
			for each (var element in storArr)
				totalBars += element[1];
			if (totalBars < numStored) {
				var barExists = false;
				for (j=0;j<storArr.length;j++) {
					if (storArr[j][0] == item.vnum) {
						storArr[j][1] = storArr[j][1] + 1;
						barExists = true;
						break;
					}
				}
				//here.echo(barExists);
				if (barExists == false) {
					storArr.push([item.vnum,1]);
				}
				storArr = storArr.join("~");
				//here.echo("new PVAL: "+storArr);
				self.say("That'll be"+actor.numCopperToText(price, true)+" when you want it back!");
				act("$n opens a locker and tosses "+item.name+" into it.",true,self,null,null,constants.TO_ROOM);
				actor.setPval("barStorage",storArr,true);
				item.moveToRoom(getRoom(20899));
				item.extract();
				return;
				
			}
			else {	
				self.say("I don't have room for anything else from you!");
				self.comm("give "+item.namelist.split(" ")[0]+" "+actor.name);
				if (self.hasItem(item,true,true,true))
					self.comm("drop "+item.namelist.split(" ")[0]);
					return;
			}
		}
	}
	else {
		self.say("Sorry, I don't deal in these.");
        self.comm("give "+item.namelist.split(" ")[0]+" "+actor.name);
        if (self.hasItem(item,true,true,true))
                self.comm("drop "+item.namelist.split(" ")[0]);
        return;
	}
}
setTimeout(1,afterWait,[actor,self,item]);

};

