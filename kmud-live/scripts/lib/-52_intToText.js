//Rhollor January 2010
//intToText(int) takes 5->"five"
//Capable of printing 0-9999
function intToText(num) {
	if(!isNumber(num)) {
		mudLog(3,100,"intToText() passed a non-number.");
	}
	var str = "";
	var teen = false;
	var ones, tens, hundreds, thousands;
	thousands = Math.floor((num%10000)/1000);
	hundreds = Math.floor((num%1000)/100);
	tens = Math.floor((num%100)/10);
	ones = num%10;
	//str += "tens: " + tens + " ones: " + ones + " hunderds: " + hundreds + " ";
	function oneToNine(digit) {
		switch(digit) {
				case 0:
					break;
				case 1:
					str += "one";
					break;
				case 2:
					str += "two";
					break;
				case 3:
					str += "three";
					break;
				case 4:
					str += "four";
					break;
				case 5:
					str += "five";
					break;
				case 6:
					str += "six";
					break;
				case 7:
					str += "seven";
					break;
				case 8:
					str += "eight";
					break;
				case 9:
					str+= "nine";
					break;
		}
	}
	oneToNine(thousands);
	if(thousands) {
		str += " thousand ";
	}
	oneToNine(hundreds);
	if(hundreds) {
		str += " hundred ";
	}
	switch(tens) {
			case 0:
				break;
			case 1:
				if((num%100) == 10)
					str += "ten";
				teen = true;
				break;
			case 2:
				str += "twenty";
				break;
			case 3:
				str += "thirty";
				break;
			case 4:
				str += "forty";
				break;
			case 5:
				str += "fifty";
				break;
			case 6:
				str += "sixty";
				break;
			case 7:
				str += "seventy";
				break;
			case 8:
				str += "eighty";
				break;
			case 9:
				str+= "ninety";
				break;
	}
	if(ones != 0 && tens > 1) {
		str += "-"
	}
	switch(ones) {
		case 0:
			if(num == 0) {
				str += "zero";
			}
			break;
		case 1:
			if(teen)
				str += "eleven";
			else
				str += "one";
			break;
		case 2:
			if(teen)
				str += "twelve";
			else
				str += "two";
			break;
		case 3:
			if(teen)
				str += "thirteen";
			else
				str += "three";
			break;
		case 4:
			str += "four";
			if(teen)
				str += "teen";
			break;
		case 5:
			if(teen)
				str += "fifteen";
			else
				str += "five";
			break;
		case 6:
			str += "six";
			if(teen)
				str += "teen";
			break;
		case 7:
			str += "seven";
			if(teen)
				str += "teen";
			break;
		case 8:
			str += "eight";
			if(teen)
				str += "teen";
			break;
		case 9:
			str += "nine";
			if(teen)
				str += "teen";
			break;
	}
	return str;
}
