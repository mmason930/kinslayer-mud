// Converts constants.RACE_XXX to "xxx"
// Rhollor - Jan 2010
function raceNumToText(raceNum) {
        var str = "undefined";
        if(raceNum == constants.RACE_TROLLOC) {
                str = "trolloc";
        }
        else if(raceNum == constants.RACE_HUMAN) {
                str = "human";
        }
        else if(raceNum == constants.RACE_SEANCHAN) {
                str = "seanchan";
        }
        else if(raceNum == constants.RACE_AIEL) {
                str = "aiel";
        }
		else if(raceNum == 5) {
			str = "animal";
        }
        return str;
}
