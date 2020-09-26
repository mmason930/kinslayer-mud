let script561 = function(self, actor, here, args, extra) {

	setTimeout(1, function() {
		let wA2 = [
			"growth",
			"decline",
			"prospects",
			"acceleration",
			"threat",
			"expansion",
			"oneness",
			"outgrowth",
			"madness",
			"evacuation",
			"diminishment",
			"consumption",
			"decay",
			"putrefaction",
			"vapidity",
			"downsizing",
			"degeneration",
			"litigation",
			"declivity",
			"hastening",
			"paradigm shifting",
			"plague"
		];

		let wA1 = [
			"phenomenal",
			"rapid",
			"chilling",
			"insipid",
			"nauseating",
			"astronomical",
			"austere",
			"inevitable",
			"inescapable",
			"reckless",
			"haphazard",
			"accelerating",
			"profound",
			"awesome",
			"terrifying",
			"ubiquitous",
			"ignominious",
			"unprecedented",
			"unparalleled",
			"insidious",
			"broad",
			"ridiculous",
			"Norum-like"
		];

		let wA3 = [
			"Ghob'lin skill",
			"local coin",
			"Borderguards attitude",
			"Aes Sedai mentality",
			"censorship",
			"Wolfbrother bonuses",
			"Gaidin equipment",
			"rampant cheating",
			"Rhollor's appearance",
			"Galnor's coding",
			"trolloc illiteracy",
			"Lithium's stabbing",
			"Osis' skill",
			"Zarth's thinking",
			"Serai's help",
			"elitist mentality",
			"the Fal Dara justice system",
			"total lack of change",
			"Gristlyn's words",
			"Kho'mon lack of resources",
			"current legends list",
			"Prism's lame tactics",
			"Norum's insanity",
			"Tyrak's suicidal intentions",
			"Gristlyn's typing skills",
			"Zyrak's brain power",
			"Jonlin's cheating",
			"Dobry's honor",
			"Atkins' reputation on Wheel of Time MUD",
			"Kho'mon reputation",
			"fades/dreadlords that cannot PK",
			"Tulon's cheating",
			"Kison's anger",
			"Kison's videos",
			"the fact that the codebase is Circle",
			"Rhollor's building skills",
			"Ravir's activeness",
			"garbage on the forums",
			"advance in technology",
			"new medical treatments"
		];

		let wA4 = [
			"undermine",
			"intensify",
			"demoralize",
			"evaporate",
			"dehumanize",
			"hasten",
			"incapacitate",
			"destabilize",
			"sabotage",
			"nullify",
			"truncate",
			"enchant",
			"enslave",
			"put an end to",
			"uglify",
			"revolutionize",
			"deteriorate",
			"overhaul",
			"aggrandize",
			"escalate",
			"emancipate"
		];

		let wA5 = [
			"today's",
			"tomorrow's",
			"the entrenchment of our",
			"yesterday's",
			"our children's",
			"modern",
			"all of our",
			"our future",
			"our",
			"the demise of our",
			"our grandchildren's",
			"all hope for",
			"the next dimension's"
		];

		let wA6 = [
			"trolloc",
			"re-ruralization",
			"self-worth",
			"trolloc condition",
			"self-esteem",
			"player psyche",
			"human psyche",
			"mental depth",
			"players attitude",
			"current sexual trends",
			"future on this mud",
			"hopes and dreams",
			"future conditions",
			"political climate",
			"lack of education",
			"cultural inadequecy",
			"sexual lifestyles",
			"greed, need, and stinginess",
			"genetic diversity",
			"player mentality",
			"mental stability"
		];

		let w1 = wA1[random(0,wA1.length-1)];
		let w2 = wA2[random(0,wA2.length-1)];
		let w3 = wA3[random(0,wA3.length-1)];
		let w4 = wA4[random(0,wA4.length-1)];
		let w5 = wA5[random(0,wA5.length-1)];
		let w6 = wA6[random(0,wA6.length-1)];

		self.say("The " + w1 + " " + w2 + " of " + w3 + " will " + w4 + " " + w5 + " " + w6 + ", " + actor.name);
	});
};