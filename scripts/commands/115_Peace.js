var script115 = function(self, actor, here, args, extra) {
	/******************************************************
	 *                                                    *
	 * Command: Peace                                     *
	 * Purpose: Stops all fighting in the immortal's room *
	 * Date: 2011-01-1                                    *
	 * Author: Galnor                                     *
	 ******************************************************/
	_block;
	if(actor.level < 103) {
		actor.send("You cannot use that command.");
		return;
	}
	here.echo("Everything is quite peaceful now.");
	var people = here.people;
	for(var index = 0;index < people.length;++index) {
		
		var vict = people[index];

		if(vict.level > actor.level) {
			continue;
		}

		if(vict.fighting) {

			if(vict.level <= actor.level) {
				vict.stopFighting();

			//	if(vict.level < actor.level) {
			//		vict.position = constants.POS_SITTING;
			//	}
			}
		}
	}
};