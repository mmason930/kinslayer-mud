var script20883 = function(self, actor, here, args, extra) {
	//Storage / Withdraw for bars at blacksmiths
	//Koradin - June 2011
	var vArgs = getArgList(args);
	var storageList = actor.getPval("barStorage");
	//here.echo("current PVAL: "+storageList);
	if (!strn_cmp(vArgs[0],"storage",3) && !strn_cmp(vArgs[0],"withdraw",3)) {
		_noblock;
		return;
	}
	_block;
	if (!storageList) {
		self.say("I don't have anything stored for you, "+actor.name+".");
		return;
	}
	storageList = storageList.split("~");
	var storArr = [];
	var itemVnum = null;
	var itemCount = null;
	for (i=0;i<storageList.length;i++) {
		storageList[i] = storageList[i].split(",");
		itemVnum = parseInt(storageList[i][0]);
		itemCount = parseInt(storageList[i][1]);
		storArr.push([itemVnum,itemCount]);
	}
	if (strn_cmp(vArgs[0],"storage",3)) {
		self.comm("nod");
		actor.send(capFirstLetter(self.name)+ " shows you a list of items you have stored:");
		act("$n shows $N a list of items.",true,self,null,actor,constants.TO_NOTVICT);
		actor.send("_________________________________________________________________________");
		actor.send(" ");
		getCharCols(actor);
		for (j=0;j<storArr.length;j++) {
			if (isName("brick",getObjProto(storArr[j][0]).name))
				var price = getObjProto(storArr[j][0]).cost/10;
			else
				var price = getObjProto(storArr[j][0]).cost;
			var space = "";
			for (k=0;k<33-getObjProto(storArr[j][0]).name.length;k++){space+=" ";}
			actor.send(cyn+(j+1)+nrm+") "+getObjProto(storArr[j][0]).name+" ["+cyn+storArr[j][1]+nrm+"]"+space+actor.numCopperToText(price,true) +" per item due");
		}
		actor.send("_________________________________________________________________________");
		actor.send(" ");
		actor.send("To withdraw an item, type "+cyn+"withdraw #"+nrm+".");
		actor.send(" ");
	}
	else if (strn_cmp(vArgs[0],"withdraw",3)) {
		if (!vArgs[1] || vArgs[2]) {
			getCharCols(actor);
			actor.send("To withdraw an item from storage, type "+cyn+"withdraw #"+nrm+".");
			return;
		}
		for (j=0;j<storArr.length;j++) {
			if (isName("brick",getObjProto(storArr[j][0]).name))
				var price = getObjProto(storArr[j][0]).cost/10;
			else
				var price = getObjProto(storArr[j][0]).cost;
			var comp = j+1;
			if (vArgs[1] == comp) {
				if (actor.gold < price) {
					self.say("Come back with the cash, "+actor.name+"!");
					return;
				}
				actor.gold -= price;
				actor.loadObj(storArr[j][0]);
				act("$n takes $N's coins and hands $N "+getObjProto(storArr[j][0]).name+".",true,self,null,actor,constants.TO_NOTVICT);
				act("$n takes your coins and hands you "+getObjProto(storArr[j][0]).name+".",true,self,null,actor,constants.TO_VICT);
				//here.echo(storArr[j][1]);
				if (storArr[j][1] == 1) {
					storArr.splice(j,1);
					if (storArr.length == 0)
						actor.deletePval("barStorage",true);
				}
				else {
					storArr[j][1] = (storArr[j][1] - 1);
				}
				storArr = storArr.join("~");
				actor.deletePval("barStorage",true);
				//here.echo("new PVAL: "+storArr);
				actor.setPval("barStorage",storArr,true);
				return;
			}
		}
		self.say("I don't have that stored for you, "+actor.name+".");
	}
}