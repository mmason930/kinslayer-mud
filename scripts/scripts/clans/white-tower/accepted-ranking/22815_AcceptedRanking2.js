var script22815 = function(self, actor, here, args, extra) {
	if( !actor.isAesSedai() ) {
		return;
	}
	let vArgs = getArgList(args);
	if( !str_cmp(vArgs[0], "novice") &&
		!str_cmp(vArgs[2], "is") &&
		!str_cmp(vArgs [3], "ready") &&
		!str_cmp(vArgs[4], "for") &&
		!str_cmp(vArgs[5], "her") &&
		!str_cmp(vArgs [6], "accepted") && !str_cmp(vArgs[7], "test") &&
		(!str_cmp(vArgs[8], "sister") || !str_cmp(vArgs[8], "sister."))
	) {
		wait 1;
		let noviceName = vArgs[1];
		let novice = getCharInListVis(self, noviceName, here.people);
		if( !novice ) {
			self.say("Really? Because I do not see " + capFirstLetter(noviceName) + " here.");
			return;
		}

		let noviceFullName = novice.name;
		const whiteDressItemVnum = 22305;

		if(!novice.inClan(constants.CLAN_WHITE_TOWER)) {

			self.comm("arch")
			waitpulse 5;
			self.say(noviceFullName + " is not a member of the White Tower.")
			return;
		}

		if(novice.getRank(constants.CLAN_WHITE_TOWER) < 2) {
			self.say(noviceFullName + " does not appear to me to be ready.");
			return;
		}

		// The dress is needed later on in the series, to be handed over to Sheriam, otherwise the Novice will
		// get stuck in the chambers and unable to proceed.
		let noviceWornAboutBody = novice.eq(constants.WEAR_ABOUT);
		if(!noviceWornAboutBody || noviceWornAboutBody.vnum !== whiteDressItemVnum) {
			self.comm("look " + noviceFullName);
			waitpulse 5;
			self.say("Why is " + noviceFullName + " not wearing her dress? Put it on immediately or we'll be unable to proceed.");
			return;
		}

		self.say("Very well then, I will alert Sheriam immediately.");
		wait 8;

		let sheriam = here.loadMob(22317);
		setSval(sheriam,22815,"novice", novice);
		here.echo("Sheriam enters the room, her blue shawl draped carefully over her shoulders.");
		sheriam.comm("phaseone")
	}
}