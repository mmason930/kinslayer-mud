//Alder
//June 2011
/** Returns a random direction constant that leads to one of the room's exits **/
JSRoom.prototype.getRandomExitDirection = function () {
	var exits = this.neighbors;
	for ( var i = 0; i < exits.length; i++ ) {
		if ( exits[i] == undefined ) {
			exits.splice(i,1);
			i--;
		}
	}
	return this.firstStep(exits[random(0,exits.length-1)]);
}
