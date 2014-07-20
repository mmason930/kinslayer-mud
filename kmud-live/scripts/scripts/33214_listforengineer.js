var script33214 = function(self, actor, here, args, extra) {
	_block;
	getCharCols(actor);
	var vArgs = getArgList(args);
	function getAvailableManorList(actor) {
		var finalArr = [];
		for (var _autoKey in global.manors) {
			var manor = global.manors[_autoKey];
			if (manor.hasGate && !manor.gateIsBroken && !manor.isUnderSiege) {
			/** var online = false;
				for (var _autoKey in getCurrentPlayers()) {
					var player = getCurrentPlayers()[_autoKey];
					if (getSval(player,33214,"hired") != manor.id) {
						if (player.id == manor.ownerUserId || arrContains(manor.allowedUsers, player.id))
							online = true;
					}
				}
				if (online == true) **/
					finalArr.push(manor);
			}
		}
		return finalArr;
	}
	if (strn_cmp(vArgs[0],"list",2)) {
		act("$N talks quietly with $n.",true,self,null,actor,constants.TO_NOTVICT);
		if (getAvailableManorList(actor).length == 0) {
			self.tell(actor,"My siege teams aren't deployed near any properties right now, "+actor.name+".");
			return;
		}
		self.tell(actor, "I currently have siege teams positioned near the following properties.");
		actor.send("\n"+capFirstLetter(self.name) +" shows you a hastily scribbled parchment.");
		act("$n shows $N a scribbled piece of parchment.",true,self,null,actor,constants.TO_NOTVICT);
		actor.send("_______________________________________________________________________\n");
		var manors = getAvailableManorList(actor);
		for (var i=0; i< manors.length; i++) {
			actor.send(cyn+(i+1)+nrm+") "+strPadding("The "+manors[i].name+" near "+manors[i].area," ",50,"right")+"Price:"+actor.numCopperToText(global.siegeCost,true));
		}
		actor.send("_______________________________________________________________________\n");
		actor.send("To hire a siege team, type BUY "+cyn+"#"+nrm+".");
	}
	else if (strn_cmp(vArgs[0],"buy",3)) {
		if (!vArgs[1] || isNaN(vArgs[1])) {
			actor.send("To hire a siege team, type BUY #.");
			actor.send("To find out which numbered property you want, type LIST.");
			return;
		}
		if (actor.gold < global.siegeCost) {
			self.tell(actor, "You don't have the cash!");
			return;
		}
		var manors = getAvailableManorList(actor);
		if (manors.length == 0) {
			self.tell(actor,"I'm sorry, but I have no siege teams available right now.");
			return;
		}
		for (var i = 0; i < manors.length; i++) {
			if (vArgs[1] == (i+1)) {
				var manor = manors[i];
			}
		}
		if (!manor) {
			actor.send("To hire a siege team, type BUY #.");
			actor.send("To find out which numbered property you want, type LIST.");
			return;
		}
		if (getSval(actor,33214,"hired") > 0) {
			self.tell(actor,"You've already hired one of my teams!");
			return;
		}
		setSval(actor,33214,"hired",manor.id);
		actor.gold -= global.siegeCost;
		self.comm("nod "+actor.name);
		self.tell(actor,"I'll send word to my team near "+manor.area+" and they will report to you when they're ready.");
		act("$n scribbles a message on a slip of parchment, secures it to a blackbird and releases it.",true,self,null,null,constants.TO_ROOM);
		wait 2;
		act("You see the blackbird wheel its way into the sky and out of sight.",true,self,null,null,constants.TO_ROOM);
		var wTime = self.room.distanceTo(getRoom(manor.outsideGateRoomVnum))/2;
		if (!wTime || wTime < 30)
			wTime = 30;
		wait wTime;
		loadSiegeTeam(actor, manor);
	}
	
	
}