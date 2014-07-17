// Rhollor January 2010
// This will take an int numCopper and convert it to text
// Sample: numCopper = 12500 -> "1 gold, 25 silver"
// dispFull is an optional boolean specifying whether or not the output will display placeholders:
// Ex. "0 gold, 32 silver, 0 copper"
JSCharacter.prototype.numCopperToText = function(numCopper, color, dispFull) {
	var nrm = this.normal(constants.CL_OFF);
	if(color) {
		var ylw = this.yellow(constants.CL_OFF);
		var cyn = this.cyan(constants.CL_OFF);
		var bld = this.bold(constants.CL_OFF);
	}
	else {
		var ylw = "";
		var cyn = "";
		var bld = "";
		var nrm = "";
	}
	var str = "";
	var gold, silver, copper;
	gold = Math.floor(numCopper/10000);
	numCopper = numCopper%10000;
	silver = Math.floor(numCopper/100);
	numCopper = numCopper%100;
	copper = numCopper;
	if(gold > 0 || dispFull) {
		str += " " + gold + bld + ylw + " gold" + nrm; 
	}
	if(silver > 0 || dispFull) {
		if(gold > 0 || dispFull) {
			str += ",";
		}
		str += " " + silver + bld + cyn + " silver" + nrm;
	}
	if(copper >= 0 || dispFull) {
		if(gold > 0 || silver > 0 || dispFull) {
			if(copper <= 0 && !dispFull) {
				return str;
			}
			str += ",";
		}
		str += " " + copper + ylw + " copper" + nrm;
	}
	return str;
}
