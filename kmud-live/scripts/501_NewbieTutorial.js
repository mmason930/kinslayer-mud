var script501 = function(self, actor, here, args, extra)
{
	//Newbie Tutorial
//Koradin - March 2012
var sval = getSval(actor,501,"tutorial");
var vArgs = getArgList(args);
getCharCols(actor);
if (sval == 0) {
	_block;
	return;
}
if (sval == 1) { // Initial Yes/No question from Guardian of Life
	_block;
	if (strn_cmp(vArgs[0],"no",1)) {
		actor.gsend("Okay, "+actor.name+". If you would like to take the tutorial at any time, just type TUTORIAL.");
		actor.detach(501);
		actor.qval("newbieTutorial", 1);
		return;
	}
	else if (strn_cmp(vArgs[0],"yes",1)) {
		actor.hunger = 24;
		actor.thirst = 24;
		actor.mood = constants.MOOD_BRAVE;
		actor.detach(501);
		actor.comm("nonar");
		actor.comm("nochat");
		actor.comm("noglo");
		actor.attach(501);
		setSval(actor,501,"tutorial",0);
		actor.deletePval("NEWBIE_KIT",true);
		for (i=0;i<22;i++) {
			if (actor.eq(i))
				actor.eq(i).moveToRoom(actor.room);
		}
		for each (var thing in actor.inventory)
			thing.moveToRoom(actor.room);
		actor.gsend("Wonderful! In this short tutorial, you'll learn the basics of the world of Kinslayer.");
		waitpulse 13;
		actor.send("The Guardian of Life teleports you to another place.");
		act("The Guardian of Life opens a portal, and $n steps through.",true,actor,null,null,constants.TO_ROOM);
		actor.moveToRoom(getRoom(8));
		waitpulse 13;
		getCharCols(actor);
		actor.gsend("To see where you are, type "+nrm+"["+cyn+"L"+nrm+"]"+cyn+bld+"ook.");
		setSval(actor,501,"tutorial",2);
		actor.send(" ");
		return;
	}
	else {
		actor.send("Please type ["+cyn+"Y"+nrm+"]es or ["+cyn+"N"+nrm+"]o.");
		return;
	}
}
else if (sval == 2) { // Learning the LOOK command
	if (strn_cmp(vArgs[0],"look",1)) {
		_noblock;
		setSval(actor,501,"tutorial",3);
		waitpulse 5;
		getCharCols(actor);
		actor.gsend("Very good! Each room has a "+nrm+cyn+"Title"+bld+", "+nrm+"Description"+cyn+bld+", and "+nrm+cyn+"Exits"+bld+". It may also have "+nrm+grn+"Items"+nrm+cyn+bld+" and "+nrm+yel+"People"+nrm+cyn+bld+".");
		waitpulse 13;
		actor.gsend("Feel free to practice any commands I teach you to get better acquainted.");
		waitpulse 17;
		getCharCols(actor);
		actor.gsend("Checking your "+nrm+"["+cyn+"SC"+nrm+"]"+cyn+bld+"ore will tell you more about yourself. Try it now.");
		actor.send(" ");
		actor.thirst = 0;
		return;
	}
	else {
		_block;
		actor.send("Type ["+cyn+"L"+nrm+"]ook to see where you are.");
		return;
	}
}
else if (sval == 3) { // Learning SCORE and STAT commands
	if (strn_cmp(vArgs[0],"look",1)) {
		_noblock;
		waitpulse 1;
		actor.send("Check your ["+cyn+"SC"+nrm+"]ore to move on to the next lesson.");
		return;
	}
	else if (strn_cmp(vArgs[0],"score",2)) {
		_noblock;
		waitpulse 5;
		getCharCols(actor);
		actor.gsend("Your hit"+(actor.class == constants.CLASS_CHANNELER ? ", spell," : "")+" and movement points are also reflected in your prompt: "+nrm+"HP:Healthy "+(actor.class == constants.CLASS_CHANNELER ? "SP:Full " : "")+"MV:Fresh >"+cyn+bld);
		setSval(actor,501,"tutorial",4);
		waitpulse 17;
		getCharCols(actor);
		actor.gsend("You can see from your score that you are thirsty.  To drink from the fountain here, type "+nrm+"["+cyn+"DR"+nrm+"]"+cyn+bld+"ink "+nrm+"["+cyn+"FOUNTAIN"+nrm+"]"+cyn+bld+".");
		actor.send(" ");
		return;
	}
	else {
		_block;
		actor.send("Check your ["+cyn+"SC"+nrm+"]ore to move on to the next lesson.");
		return;
	}
}
else if (sval == 4) { // Learning to DRINK
	if (strn_cmp(vArgs[0],"look",1) || strn_cmp(vArgs[0],"score",2) || strn_cmp(vArgs[0],"stats",4)) {
		_noblock;
		waitpulse 1;
		actor.send("["+cyn+"DR"+nrm+"]ink from the ["+cyn+"FOUNTAIN"+nrm+"]" + " to move on to the next lesson.");
		return;
	}
	else if (strn_cmp(vArgs[0],"drink",2)) {
		if (!vArgs[1]) {
			_block;
			actor.send("["+cyn+"DR"+nrm+"]ink from the ["+cyn+"FOUNTAIN"+nrm+"]" + " to move on to the next lesson.");
			return;
		}
		else if (str_cmp(vArgs[1],"fountain")) {
			_block;
			actor.send("["+cyn+"DR"+nrm+"]ink from the ["+cyn+"FOUNTAIN"+nrm+"]" + " to move on to the next lesson.");
			return;
		}
		else {
			_noblock;
			setSval(actor,501,"tutorial",5);
			waitpulse 5;
			actor.gsend("Here is a water flask so you can drink on the go.");
			actor.send("The Guardian of Life gives you a light leather flask.");
			actor.loadObj(2021);
			waitpulse 13;
			getCharCols(actor);
			actor.gsend("To see what you're carrying, check your "+nrm+"["+cyn+"I"+nrm+"]"+cyn+bld+"nventory.");
			actor.send(" ");
			return;
		}
	}
	else {
		_block;
		actor.send("["+cyn+"DR"+nrm+"]ink from the ["+cyn+"FOUNTAIN"+nrm+"]" + " to move on to the next lesson.");
		return;
	}
}
else if (sval == 5) { // Learning the INVENTORY command
	if (strn_cmp(vArgs[0],"drink",2) || strn_cmp(vArgs[0],"look",1) || strn_cmp(vArgs[0],"score",2) || strn_cmp(vArgs[0],"stats",4)) {
		_noblock;
		waitpulse 1;
		actor.send("Check your ["+cyn+"I"+nrm+"]nventory to move on to the next lesson.");
		return;
	}
	else if (strn_cmp(vArgs[0],"inventory",1)) {
		_noblock;
		setSval(actor,501,"tutorial",6);
		waitpulse 5;
		actor.gsend("Check your inventory and stats often to make sure you're not carrying something heavy - it will lower your defense!");
		waitpulse 13;
		getCharCols(actor);
		actor.gsend("Now, to venture into the world of Kinslayer, you'll need some equipment.");
		waitpulse 13;
		actor.gsend("Each time you die, you can "+nrm+"["+cyn+"TELL GUARDIAN KIT"+nrm+"]"+cyn+bld+" to receive some starting equipment. Try it now.");
		actor.send(" ");
		return;
	}
	else {
		_block;
		actor.send("Check your ["+cyn+"I"+nrm+"]nventory to move on to the next lesson.");
		return;
	}
}
else if (sval == 6) { // Learning the TELL GUARDIAN KIT command
	if (strn_cmp(vArgs[0],"inventory",1) || strn_cmp(vArgs[0],"drink",2) || strn_cmp(vArgs[0],"look",1) || strn_cmp(vArgs[0],"score",2) || strn_cmp(vArgs[0],"stats",4)) {
		_noblock;
		waitpulse 1;
		actor.send("["+cyn+"TELL GUARDIAN KIT"+nrm+"] to receive some basic equipment.");
		return;
	}
	else if (strn_cmp(vArgs[0],"tell",1) && strn_cmp(vArgs[1],"guardian",8) && strn_cmp(vArgs[2],"kit",3)) {
		_noblock;
		setSval(actor,501,"tutorial",7);
		waitpulse 13;
		getCharCols(actor);
		actor.gsend("The equipment won't do much good if you're carrying it, so "+nrm+"["+cyn+"WEAR ALL"+nrm+"]"+cyn+bld+" to wear it.");
		actor.send(" ");
		return;
	}
	else {
		_block;
		actor.send("["+cyn+"TELL GUARDIAN KIT"+nrm+"] to receive some basic equipment.");
		return;
	}
}
else if (sval == 7) { // Learning the WEAR ALL command
	if (strn_cmp(vArgs[0],"tell",1) || strn_cmp(vArgs[0],"inventory",1) || strn_cmp(vArgs[0],"drink",2) || strn_cmp(vArgs[0],"look",1) || strn_cmp(vArgs[0],"score",2) || strn_cmp(vArgs[0],"stats",4)) {
		_noblock;
		waitpulse 1;
		actor.send("["+cyn+"WEAR ALL"+nrm+"] to equip the gear the Guardian of Life gave you.");
		return;
	}
	else if (strn_cmp(vArgs[0],"wear",4) && strn_cmp(vArgs[1],"all",3)) {
		_noblock;
		setSval(actor,501,"tutorial",8);
		waitpulse 5;
		actor.gsend("Since you're doing so well so far, I think I'll reward you with a better weapon!");
		if (actor.class == constants.CLASS_CHANNELER) {
			var newWeapon = 1115;
			var newName = "SPEAR";
		}
		else if (actor.class == constants.CLASS_THIEF) {
			var newWeapon = 1410;
			var newName = "KNIFE";
		}
		else if (actor.class == constants.CLASS_RANGER) {
			var newWeapon = 1318;
			var newName = "SWORD";
		}
		else {
			var newWeapon = 1606;
			var newName = "MACE";
		}
		actor.send("The Guardian of Life gives you "+getObjProto(newWeapon).name+".");
		actor.loadObj(newWeapon);
		waitpulse 13;
		getCharCols(actor);
		actor.gsend(nrm+"["+cyn+"REM"+nrm+"]"+cyn+bld+"ove your "+nrm+"["+cyn+"DAGGER"+nrm+"]"+cyn+bld+", then "+nrm+"["+cyn+"WIELD"+nrm+"]"+cyn+bld+" the "+nrm+"["+cyn+newName+nrm+"]"+cyn+bld+".");
		actor.send(" ");
		return;
	}
	else {
		_block;
		actor.send("["+cyn+"WEAR ALL"+nrm+"] to equip the gear the Guardian of Life gave you.");
		return;
	}
}
else if (sval == 8) { // Learning the REMOVE command
	if (actor.class == constants.CLASS_CHANNELER) {
		var newWeapon = 1115;
		var newName = "SPEAR";
	}
	else if (actor.class == constants.CLASS_THIEF) {
		var newWeapon = 1410;
		var newName = "KNIFE";
	}
	else if (actor.class == constants.CLASS_RANGER) {
		var newWeapon = 1318;
		var newName = "SWORD";
	}
	else {
		var newWeapon = 1606;
		var newName = "MACE";
	}
	if (strn_cmp(vArgs[0],"wear",3) || strn_cmp(vArgs[0],"tell",1) || strn_cmp(vArgs[0],"inventory",1) || strn_cmp(vArgs[0],"drink",2) || strn_cmp(vArgs[0],"look",1) || strn_cmp(vArgs[0],"score",2) || strn_cmp(vArgs[0],"stats",4)) {
		_noblock;
		waitpulse 1;
		actor.send("["+cyn+"REM"+nrm+"]ove your ["+cyn+"DAGGER"+nrm+"] to continue on.");
		return;
	}
	else if (strn_cmp(vArgs[0],"remove",3) && strn_cmp(vArgs[1],"dagger",6)) {
		_noblock;
		setSval(actor,501,"tutorial",9);
		waitpulse 1;
		actor.send("Now ["+cyn+"WIELD"+nrm+"] the ["+cyn+newName+nrm+"] to move on to the next lesson.");
		return;
	}
	else {
		_block;
		actor.send("["+cyn+"REM"+nrm+"]ove your ["+cyn+"DAGGER"+nrm+"] to continue on.");
		return;
	}
}
else if (sval == 9) { // Learning the WIELD command
	if (actor.class == constants.CLASS_CHANNELER) {
		var newWeapon = 1115;
		var newName = "SPEAR";
	}
	else if (actor.class == constants.CLASS_THIEF) {
		var newWeapon = 1410;
		var newName = "KNIFE";
	}
	else if (actor.class == constants.CLASS_RANGER) {
		var newWeapon = 1318;
		var newName = "SWORD";
	}
	else {
		var newWeapon = 1606;
		var newName = "MACE";
	}
	if (strn_cmp(vArgs[0],"remove",3) || strn_cmp(vArgs[0],"wear",3) || strn_cmp(vArgs[0],"tell",1) || strn_cmp(vArgs[0],"inventory",1) || strn_cmp(vArgs[0],"drink",2) || strn_cmp(vArgs[0],"look",1) || strn_cmp(vArgs[0],"score",2) || strn_cmp(vArgs[0],"stats",4)) {
		_noblock;
		waitpulse 1;
		actor.send("["+cyn+"WIELD"+nrm+"] your ["+cyn+newName+nrm+"] to move on to the next lesson.");
		return;
	}
	else if (strn_cmp(vArgs[0],"wield",2) && strn_cmp(vArgs[1],newName,newName.length)) {
		_noblock;
		setSval(actor,501,"tutorial",10);
		waitpulse 5;
		getCharCols(actor);
		actor.gsend("Now, check your "+nrm+"["+cyn+"EQ"+nrm+"]"+cyn+bld+"uipment to see what you're wearing.");
		actor.send(" ");
		return;
	}
	else {
		_block;
		actor.send("Now ["+cyn+"WIELD"+nrm+"] your ["+cyn+newName+nrm+"] to move on to the next lesson.");
		return;
	}
}
else if (sval == 10) { // Learning the EQ command
	if (strn_cmp(vArgs[0],"wield",2) || strn_cmp(vArgs[0],"remove",3) || strn_cmp(vArgs[0],"wear",3) || strn_cmp(vArgs[0],"tell",1) || strn_cmp(vArgs[0],"inventory",1) || strn_cmp(vArgs[0],"drink",2) || strn_cmp(vArgs[0],"look",1) || strn_cmp(vArgs[0],"score",2) || strn_cmp(vArgs[0],"stats",4)) {
		_noblock;
		waitpulse 1;
		getCharCols(actor);
		actor.send("Check your ["+cyn+"EQ"+nrm+"]uipment to make sure you're ready to go.");
		return;
	}
	else if (strn_cmp(vArgs[0],"equipment",2)) {
		_noblock;
		setSval(actor,501,"tutorial",11);
		waitpulse 5;
		getCharCols(actor);
		actor.gsend("Feel free to practice all of these commands at any time.");
		waitpulse 13;
		getCharCols(actor);
		actor.gsend("Head "+nrm+"["+cyn+"E"+nrm+"]"+cyn+bld+"ast and attack the brown deer there.");
		actor.send(" ");
		return;
	}
	else {
		_block;
		actor.send("Check your ["+cyn+"EQ"+nrm+"]uipment to make sure you're ready to go.");
		return;
	}
}
else if (sval == 11) { // Learning to move (EAST)
	if (strn_cmp(vArgs[0],"equipment",2) || strn_cmp(vArgs[0],"remove",3) || strn_cmp(vArgs[0],"wield",2) || strn_cmp(vArgs[0],"wear",3) || strn_cmp(vArgs[0],"tell",1) || strn_cmp(vArgs[0],"inventory",1) || strn_cmp(vArgs[0],"drink",2) || strn_cmp(vArgs[0],"look",1) || strn_cmp(vArgs[0],"score",2) || strn_cmp(vArgs[0],"stats",4)) {
		_noblock;
		waitpulse 1;
		actor.send("Head ["+cyn+"E"+nrm+"]ast to continue on.");
		return;
	}
	else if (strn_cmp(vArgs[0],"east",1)) {
		_block;
		for (var i=26; i<40; i++) {
			var inRoom = false;
			for each (var person in getRoom(i).people) {
				if (person.vnum == -1)
					inRoom = true;
			}
			if (inRoom == false) {
				var roomVnum = i;
				break;
			}
		}
		actor.moveToRoom(getRoom(roomVnum));
		actor.room.loadMob(208);
		actor.detach(501);
		actor.comm("look");
		actor.attach(501);
		setSval(actor,501,"tutorial",12);
		waitpulse 5;
		actor.gsend("The area is dark here! There were torches in the kit I gave you. Check your inventory.");
		waitpulse 13;
		getCharCols(actor);
		actor.gsend("You won't be able to see unless you "+nrm+"["+cyn+"HOLD"+nrm+"]"+cyn+bld+" a "+nrm+"["+cyn+"TORCH"+nrm+"]"+cyn+bld+".");
		return;
	}
	else {
		_block;
		actor.send("Head ["+cyn+"E"+nrm+"]ast to continue on.");
		return;
	}
}
else if (sval == 12) { // Learning to HOLD a light
	if (strn_cmp(vArgs[0],"equipment",2) || strn_cmp(vArgs[0],"east",1) || strn_cmp(vArgs[0],"remove",3) || strn_cmp(vArgs[0],"wield",2) || strn_cmp(vArgs[0],"wear",3) || strn_cmp(vArgs[0],"tell",1) || strn_cmp(vArgs[0],"inventory",1) || strn_cmp(vArgs[0],"drink",2) || strn_cmp(vArgs[0],"look",1) || strn_cmp(vArgs[0],"score",2) || strn_cmp(vArgs[0],"stats",4)) {
		_noblock;
		waitpulse 1;
		actor.send("["+cyn+"HOLD TORCH"+nrm+"] so you can see where you are.");
		return;
	}
	else if (strn_cmp(vArgs[0],"hold",2) && strn_cmp(vArgs[1],"torch",5)) {
		_noblock;
		setSval(actor,501,"tutorial",13);
		waitpulse 5;
		getCharCols(actor);
		actor.gsend("Now look to see the deer in the room. "+nrm+"["+cyn+"K"+nrm+"]"+cyn+bld+"ill "+nrm+"["+cyn+"DEER"+nrm+"]"+cyn+bld+" to attack it.");
		actor.send(" ");
		return;
	}
	else {
		_block;
		actor.send("["+cyn+"HOLD TORCH"+nrm+"] so you can see where you are.");
		return;
	}
}
else if (sval == 13) { // Learning to KILL a deer
	if (strn_cmp(vArgs[0],"equipment",2) || strn_cmp(vArgs[0],"hold",2) || strn_cmp(vArgs[0],"east",1) || strn_cmp(vArgs[0],"remove",3) || strn_cmp(vArgs[0],"wield",2) || strn_cmp(vArgs[0],"wear",3) || strn_cmp(vArgs[0],"tell",1) || strn_cmp(vArgs[0],"inventory",1) || strn_cmp(vArgs[0],"drink",2) || strn_cmp(vArgs[0],"look",1) || strn_cmp(vArgs[0],"score",2) || strn_cmp(vArgs[0],"stats",4)) {
		_noblock;
		waitpulse 1;
		if (!actor.fighting)
			actor.send("["+cyn+"K"+nrm+"]ill the ["+cyn+"DEER"+nrm+"] to move on to the next lesson.");
		return;
	}
	else if (strn_cmp(vArgs[0],"kill",1) && strn_cmp(vArgs[1],"deer",4)) {
		_noblock;
		setSval(actor,501,"tutorial",14);
		waitpulse 5;
		getCharCols(actor);
		actor.gsend("You can see your prompt now shows the deer's health as well as your own.");		
		waitpulse 30;
		actor.gsend("Each attempt to hit each other is called a combat round.");
		actor.send(" ");
		return;
	}
	else {
		_block;
		if (!actor.fighting)
			actor.send("["+cyn+"K"+nrm+"]ill the ["+cyn+"DEER"+nrm+"] to move on to the next lesson.");
		else
			actor.send("Wait until you're finished killing the deer to move on.");
		return;
	}
}
else if (sval == 14) { // Learning to EAT part 1
	if (strn_cmp(vArgs[0],"equipment",2) || strn_cmp(vArgs[0],"hold",2) || strn_cmp(vArgs[0],"east",1) || strn_cmp(vArgs[0],"remove",3) || strn_cmp(vArgs[0],"wield",2) || strn_cmp(vArgs[0],"wear",3) || strn_cmp(vArgs[0],"tell",1) || strn_cmp(vArgs[0],"inventory",1) || strn_cmp(vArgs[0],"drink",2) || strn_cmp(vArgs[0],"look",1) || strn_cmp(vArgs[0],"score",2) || strn_cmp(vArgs[0],"stats",4)) {
		_noblock;
		return;
	}
	else if (strn_cmp(vArgs[0],"imdonewiththedeer",17)) {
		_block;
		setSval(actor,501,"tutorial",15);
		actor.hunger = 0;
		waitpulse 5;
		getCharCols(actor);
		actor.gsend("Nicely done! Many corpses can be butchered for meat. For now, the kit I gave you includes some meat.");		
		waitpulse 13;
		getCharCols(actor);
		actor.gsend("Check your score to make sure you're hungry, then "+nrm+"["+cyn+"EAT MEAT"+nrm+"]"+cyn+bld+".");
		actor.send(" ");
		return;
	}
	else {
		_block;
		actor.send("["+cyn+"EAT MEAT"+nrm+"] to move on to the next lesson.");
		return;
	}
}
else if (sval == 15) { // Learning to EAT part 2
	if (strn_cmp(vArgs[0],"equipment",2) || strn_cmp(vArgs[0],"hold",2) || strn_cmp(vArgs[0],"east",1) || strn_cmp(vArgs[0],"remove",3) || strn_cmp(vArgs[0],"wield",2) || strn_cmp(vArgs[0],"wear",3) || strn_cmp(vArgs[0],"tell",1) || strn_cmp(vArgs[0],"inventory",1) || strn_cmp(vArgs[0],"drink",2) || strn_cmp(vArgs[0],"look",1) || strn_cmp(vArgs[0],"score",2) || strn_cmp(vArgs[0],"stats",4)) {
		_noblock;
		waitpulse 1;
		getCharCols(actor);
		actor.send("["+cyn+"EAT MEAT"+nrm+"] to move on to the next lesson.");
		return;
	}
	else if (strn_cmp(vArgs[0],"eat",3) && strn_cmp(vArgs[1],"meat",4)) {
		_noblock;
		setSval(actor,501,"tutorial",16);
		waitpulse 5;
		actor.gsend("It's best to eat and drink multiple times until you're full. Supplies like flasks, meat and lanterns can be purchased from shops in most cities.");		
		waitpulse 13;
		getCharCols(actor);
		actor.gsend("You're almost done! Head "+nrm+"["+cyn+"W"+nrm+"]"+cyn+bld+"est and we'll finish up.");
		actor.send(" ");
		return;
	}
	else {
		_block;
		getCharCols(actor);
		actor.send("["+cyn+"EAT MEAT"+nrm+"] to move on to the next lesson.");
		return;
	}
}
else if (sval == 16) { // Learning to move (WEST)
	if (strn_cmp(vArgs[0],"eat",3) || strn_cmp(vArgs[0],"equipment",2) || strn_cmp(vArgs[0],"hold",2) || strn_cmp(vArgs[0],"east",1) || strn_cmp(vArgs[0],"remove",3) || strn_cmp(vArgs[0],"wield",2) || strn_cmp(vArgs[0],"wear",3) || strn_cmp(vArgs[0],"tell",1) || strn_cmp(vArgs[0],"inventory",1) || strn_cmp(vArgs[0],"drink",2) || strn_cmp(vArgs[0],"look",1) || strn_cmp(vArgs[0],"score",2) || strn_cmp(vArgs[0],"stats",4)) {
		_noblock;
		waitpulse 1;
		actor.send("Head ["+cyn+"W"+nrm+"] to meet up with the Guardian of Life.");
		return;
	}
	else if (strn_cmp(vArgs[0],"west",1)) {
		_noblock;
		setSval(actor,501,"tutorial",17);
		waitpulse 5;
		actor.gsend("Welcome back, "+actor.name+". Our last lesson is on communication. One of the best ways to ask for help is to find and meet other players.");		
		waitpulse 13;
		getCharCols(actor);
		actor.gsend("The most basic way to talk is to just "+nrm+"["+cyn+"SAY HELLO"+nrm+"]"+cyn+bld+". Try it now.");
		actor.send(" ");
		return;
	}
	else {
		_block;
		actor.send("Head ["+cyn+"W"+nrm+"] to meet up with the Guardian of Life.");
		return;
	}
}
else if (sval == 17) { // Learning to communicate (SAY)
	if (strn_cmp(vArgs[0],"west",1) || strn_cmp(vArgs[0],"eat",3) || strn_cmp(vArgs[0],"equipment",2) || strn_cmp(vArgs[0],"hold",2) || strn_cmp(vArgs[0],"east",1) || strn_cmp(vArgs[0],"remove",3) || strn_cmp(vArgs[0],"wield",2) || strn_cmp(vArgs[0],"wear",3) || strn_cmp(vArgs[0],"tell",1) || strn_cmp(vArgs[0],"inventory",1) || strn_cmp(vArgs[0],"drink",2) || strn_cmp(vArgs[0],"look",1) || strn_cmp(vArgs[0],"score",2) || strn_cmp(vArgs[0],"stats",4)) {
		_noblock;
		waitpulse 1;
		actor.send("["+cyn+"SAY HELLO"+nrm+"] to finish the tutorial.");
		return;
	}
	else if (strn_cmp(vArgs[0],"say",3) && strn_cmp(vArgs[1],"hello",5)) {
		_noblock;
		setSval(actor,501,"tutorial",0);
		waitpulse 5;
		actor.gsend(nrm + "[" + cyn + "SAY" + nrm + "]" + bld + cyn + "ing something lets anyone in the room hear it. " + nrm + "[" + nrm + cyn + "NAR" + nrm + "]" + bld + cyn + "rating or " + nrm + "[" + nrm + cyn + "CHAT" + nrm + "]" + bld + cyn + "ting lets all humans hear it, and " + nrm + "[" + nrm + cyn + "GLO" + nrm + "]" + bld + cyn + "baling something lets all players hear it.");
		waitpulse 50;
		actor.gsend("That concludes the Kinslayer tutorial! Here are a few parting suggestions that may help you.");
		waitpulse 30;
		actor.gsend("You'll begin your journey in a village called Emond's Field. Read this map if you get lost.");
		actor.send("The Guardian of Life gives you a map of Emond's Field.");
		actor.loadObj(13221);
		waitpulse 50;
		actor.gsend("You can also see where you are in the world by simply typing MAP.");
		waitpulse 30;
		actor.gsend("If you have further questions, you can type HELP <topic> to open your newbie guide. For example, HELP FOOD or HELP LANTERN.  You can also just type HELP by itself to go to your newbie guide's table of contents.");
		waitpulse 70;
		actor.gsend("Good luck, "+actor.name+", and welcome to Kinslayer!");
		waitpulse 30;
		actor.send("The Guardian of Life teleports you to another place.");
		waitpulse 17;
		actor.moveToRoom(getRoom(13226));
		actor.detach(501);	
		actor.comm("nonar");
		actor.comm("nochat");
		actor.comm("noglo");
		actor.comm("west");
		//waitpulse 20;
		//actor.moveToRoom(getRoom(500));
		//actor.send("Thanks for testing the tutorial! Emond's Field will be here soon. - Koradin");
		return;
	}
	else {
		_block;
		actor.send("["+cyn+"SAY HELLO"+nrm+"] to finish the tutorial.");
		return;
	}
}
actor.detach(501);












};

