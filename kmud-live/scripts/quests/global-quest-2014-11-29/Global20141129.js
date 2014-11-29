function Global2014Util()
{
	this.kingTurkeyVnum = 23006;
	this.followerTurkeyvnum = 23005;
}

Global2014Util.prototype.test = function()
{
};

//Random gobbling / emotes / fight events.
var script23005 = function(self, actor, here, args, extra) {

	var randomNumber = random(1, 100);

	if(randomNumber < 10)
	{
		act("$n gobbles, 'Mang...'", false, self, null, null, constants.TO_ROOM);
	}
	else if(randomNumber < 20 && self.fighting)
	{
		act("$n points at $N and gobbles deeply, 'Die mang!", false, self, null, self.fighting, constants.TO_NOTVICT);
		act("$n points at you and gobbles deeply, 'Die mang!'", false, self, null, self.fighting, constants.TO_VICT);
		act("You point at $N and gobble deeply, 'Die mang!'", false, self, null, self.fighting, constants.TO_CHAR);

		waitpulse 5;

		act("$n roundhouse kicks $N in the face!", false, self, null, self.fighting, constants.TO_NOTVICT);
		act("$n roundhouse kicks you right in the face!", false, self, null, self.fighting, constants.TO_VICT);
		act("You roundhouse kick $N right in the face!", false, self, null, self.fighting, constants.TO_CHAR);

		self.fighting.damage(random(10, 15), self);
	}
};
