//JSCharacter.heal(int hps) heals the JSCharacter hps amount
//Rhollor - January 2010
JSCharacter.prototype.heal = function(hps) {
	this.hps += hps;
	if(this.hps > this.maxHit) {
		this.hps = this.maxHit;
	}
	return;
}
