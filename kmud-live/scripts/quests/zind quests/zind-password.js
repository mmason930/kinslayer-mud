var Zind = (function() {
	var pval = "ZIND_PASS";
	var passwords = ["nevermind", "marauder", "submerged", "swift", "deuces", "highland", "cargo", "seabreeze", "behold", "keepsake", "horseshoe", "waterfall", "melon", "blade", "hilt", "pommel", "ruby", "stone", "livid", "spirit", "power", "onyx", "goldenrod", "mist", "stormy", "downpour", "jade", "mahogany", "snowfall", "sunshine", "light", "darkness", "eternity", "herons", "wounded", "backstab", "traitor", "imbecile", "foolhardy", "meatpie", "raven", "beak", "sparrow", "flaming", "demise", "irridescent", "firefly", "serenity", "absolute", "envy", "green", "red", "cherry", "apple", "bloodied", "dragon", "reborn", "creator", "Ishamael", "Lanfear", "Rahvin", "forsaken", "darkhound", "statue", "Callandor", "eyeless"];

	return {
		setNewPassword: function(actor) {
			actor.setPval(pval, passwords[_.random(0, passwords.length - 1)]);
		},

		getPassword: function(actor) {
			actor.getPval(pval);
		},

		printPassword: function(actor) {
			return actor.cyan() + this.getPassword(actor) + actor.normal();
		}
	};
})();
