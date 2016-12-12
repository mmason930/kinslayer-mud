var script18605 = function(self, actor, here, args, extra) {

	var vArgs = getArgList(args);

	_block;

	if(vArgs.length == 1)
	{
		actor.send("That person isn't here.");
		return;
	}

	var targetName = vArgs[1];
	var targetList = [];
	var target = getCharInListVis(actor, targetName, here.people);

	vArgs.splice(0, 2);
	var orderedCommand = vArgs.join(" ");

	if(!target || vArgs.length == 0) {
		actor.send("Order who to do what?");
		return;
	}
	else if(strn_cmp(targetName, "all", 1)) {
		targetList = here.people.filter(function (person) { return MercUtil.getOwner(person) == actor; });

		actor.send("You issue an order.");
		act("$n issues an order.", false, actor, null, null, constants.TO_ROOM);
	}
	else {
		targetList = [target];

		act("You issue an order to $N.", false, actor, null, target, constants.TO_CHAR);
		act("$n issues an order to $N.", false, actor, null, target, constants.TO_NOTVICT);
		act("$n orders you to '" + orderedCommand + "'", false, actor, null, target, constants.TO_VICT);
	}
	//NOTE that vArgs here has the first two words spliced from away the original script(see above).

	getCharCols(actor);

	var mercReq = function(actor, target) {
		return MercUtil.getOwner(target) == actor;
	};

	var followSelfReq = function(actor, target) {
		return target.leader = actor && target.isMount();
	};

	function followSelfOperator(actor, target) {

		target.comm("follow self");
	}

	var operationArray = [

		[followSelfReq, function(arg1, arg2) {
			return strn_cmp(arg1, "follow", 3) && !str_cmp(arg2, "self");
		}, followSelfOperator],
		[mercReq, function (arg1) {
			return strn_cmp(arg1, "crawl", 2);
		}, MercUtil.performCrawl],
		[mercReq, function (arg1) {
			return strn_cmp(arg1, "pick", 1);
		}, MercUtil.performPick],
		[mercReq, function (arg1) {
			return strn_cmp(arg1, "sneak", 2);
		}, MercUtil.performSneak],
		[mercReq, function (arg1) {
			return strn_cmp(arg1, "ride", 2);
		}, MercUtil.performRide],
		[mercReq, function (arg1) {
			return strn_cmp(arg1, "dismount", 2);
		}, MercUtil.performDismount],
		[mercReq, function (arg1) {
			return strn_cmp(arg1, "rescue", 3);
		}, MercUtil.performRescue],
		[mercReq, function (arg1) {
			return arg1 == "follow";
		}, MercUtil.performFollow],
		[mercReq, function (arg1) {
			var dir = dirFromTextAbbreviated(arg1[0]);
			if(dir == -1)
				return false;

			return strn_cmp(arg1, dirToText(dir), arg1.length);
		}, MercUtil.performMove],
		[mercReq, function (arg1) {
			return strn_cmp(arg1, "flee", 1);
		}, MercUtil.performFlee],
		[mercReq, function (arg1) {
			return strn_cmp(arg1, "shield block", 2);
		}, MercUtil.performShieldBlock],
		[mercReq, function (arg1) {
			return strn_cmp(arg1, "precise strike", 2);
		}, MercUtil.performPreciseStrike],
		[mercReq, function (arg1) {
			return strn_cmp(arg1, "bash", 2);
		}, MercUtil.performBash],
		[mercReq, function (arg1) {
			return strn_cmp(arg1, "assist", 2);
		}, MercUtil.performAssist],
		[mercReq, function (arg1) {
			return arg1 == "hide";
		}, MercUtil.performHide],
		[mercReq, function (arg1) {
			return arg1 == "open";
		}, MercUtil.performOpen],
		[mercReq, function (arg1) {
			return strn_cmp(arg1,"rest",1);
		}, MercUtil.performRest],
		[mercReq, function (arg1) {
			return strn_cmp(arg1,"stand",2);
		}, MercUtil.performStand],
		[mercReq, function (arg1) {
			return strn_cmp(arg1,"sit",2);
		}, MercUtil.performSit],
		[mercReq, function (arg1) {
			return strn_cmp(arg1,"wake",2);
		}, MercUtil.performWake],
		[mercReq, function (arg1) {
			return strn_cmp(arg1,"take",1) || strn_cmp(arg1,"get",1);
		}, MercUtil.performWake],
		[mercReq, function (arg1) {
			return strn_cmp(arg1, "drop", 2);
		}, MercUtil.performDrop],
		[mercReq, function (arg1) {
			return strn_cmp(arg1, "give", 2);
		}, MercUtil.performGive],
		[mercReq, function (arg1) {
			return strn_cmp(arg1,"remove",3);
		}, MercUtil.performRemove],
		[mercReq, function (arg1) {
			return strn_cmp(arg1,"wear",3);
		}, MercUtil.performWear],
		[mercReq, function (arg1) {
			return strn_cmp(arg1,"wield",2);
		}, MercUtil.performWield],
		[mercReq, function (arg1) {
			return strn_cmp(arg1,"hold",1) || strn_cmp(arg1,"grab",2);
		}, MercUtil.performHold],
		[mercReq, function (arg1) {
			return strn_cmp(arg1,"put",2);
		}, MercUtil.performPut],
		[mercReq, function (arg1, arg2) {
			return arg1 == "return" && arg2 == "home";
		}, MercUtil.performReturnHome],
		[mercReq, function (arg1) {
			return strn_cmp(arg1,"inventory",1);
		}, MercUtil.performInventory],
		[mercReq, function (arg1) {
			return strn_cmp(arg1,"save",2);
		}, MercUtil.performSave],
		[mercReq, function (arg1) {
			return strn_cmp(arg1,"report",3) || strn_cmp(arg1,"status",4);
		}, MercUtil.performReport],
		[mercReq, function (arg1) {
			return strn_cmp(arg1, "rendezvous", 3);
		}, MercUtil.performRendezvous],
		[mercReq, function (arg1) {
			return arg1 == "pwn";
		}, MercUtil.performPwn],
		[mercReq, function (arg1) {
			return strn_cmp(arg1,"channel",2);
		}, MercUtil.performChannel],
		[mercReq, function (arg1) {
			return strn_cmp(arg1,"commands",1);
		}, MercUtil.performCommands]
	];

	targetList.forEach(function(target) {

		for (var index = 0; index < operationArray.length;++index) {
			var operation = operationArray[index];

			if(operation[0](actor, target) && operation[1](vArgs[0], vArgs[1]))
			{
				var mercData = MercUtil.getMercenaryData(target);
				operation[2](actor, target, vArgs, here, mercData);
				break;
			}
		}
	});
};
