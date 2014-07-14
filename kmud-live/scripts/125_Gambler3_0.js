var script125 = function(self, actor, here, args, extra)
{
	if( self.isInUse )
	return;
try {//The finally clause is guaranteed to get called. It is responsible for unsetting isInUse
self.isInUse = true;
if( self.race != actor.race ) {
	self.say("I don't gamble with your kind!");
	return;
}
var vArgs = getArgList(args);
wait 1;
if( vArgs.length <= 1 ) {
	//'say gamble'
	self.say("Why, yes. I am a gambler of course!");
	wait 1;
	self.say("You may say, 'gamble <coppers>', where coppers is the amount you wish to wager.");
	wait 1;
	self.say("We will then place our money on the table, and play a quick game of dice.");
	wait 1;
	self.say("If you'd like to be informed of the records, simply say, 'gamble records'.");
	return;
}
function randomFreakout( gambler ) {
	var rand = random(1,4);
	if( rand == 1 )
		gambler.say("NOOOOOO!!");
	else if( rand == 2 )
		gambler.comm("scream");
	else if( rand == 3 )
		gambler.comm("cry");
	else if( rand == 4 )
		gambler.comm("facepalm");
}
function randomBrag( gambler ) {
	var rand = random(1,4);
	if( rand == 1 )
		gambler.say("Yes!!!");
	else if( rand == 2 )
		gambler.comm("cheer");
	else if( rand == 3 )
		gambler.comm("strut");
	else if( rand == 4 )
		gambler.comm("yodel");
}
function getRecordFileName( gambler ) {
	return "gamblerRecords" + gambler.room.vnum;
}
function getRecordFileContents( gambler ) {
	return fread( getRecordFileName( gambler ) );
}
function setRecordFileContents( gambler, newContents ) {
	fwrite( getRecordFileName(gambler), newContents );
}
function loadRecords(gambler) {
	var recordObject = new Object();
	var fileContent = getRecordFileContents(gambler);
	
	var vData = fileContent.split("~");
	
	if( vData.length >= 3 ) {
		//First three elements store the overall record.
		var record = new Object();
		record.time = parseInt(vData[0]);
		record.amount = parseInt(vData[1]);
		record.playerName = vData[2];
		recordObject.overall = record;
	}
	return recordObject;
}
function saveRecords(gambler,recordObject) {
	var buffer = "";
	var fileName = getRecordFileName(gambler);
	
	if( recordObject.overall ) {
		buffer += recordObject.overall.time;
		buffer += "~";
		buffer += recordObject.overall.amount;
		buffer += "~";
		buffer += recordObject.overall.playerName;
	}
	fwrite(fileName,buffer);
}
if( !str_cmp(vArgs[1],"records") ) {
	var records = loadRecords( self );
	
	var sayBuffer = "";
	sayBuffer += "Overall : ";
	
	if( !records.overall ) {
		sayBuffer += "<None>";
	}
	else {
		var daysSince = time() - records.overall.time;
		daysSince /= (60*60*24);
		daysSince = Math.floor(daysSince);
		
		if( daysSince == 0 )
			daysSince = "today";
		else if( daysSince == 1 )
			daysSince = "yesterday";
		else
			daysSince = daysSince + " days ago.";
		sayBuffer += records.overall.playerName + " raked in " + records.overall.amount + " coppers " + daysSince + ". That little swine!";
	}
	
	self.say( sayBuffer  );
}
else {
	if( !isNumber( vArgs[1] ) ) {
		self.say("I only gamble in coppers, " + (actor.sex == constants.SEX_MALE ? "man" : "toots") + ".");
		return;
	}
	var coppersToGamble = parseInt(vArgs[1]);
	if( actor.gold < coppersToGamble ) {
		self.say("Maybe when you're not so broke.");
		return;
	}
	if( coppersToGamble <= 0 ) {
		self.say("Stop wasting my time with nonsense.");
		return;
	}
	actor.gold -= coppersToGamble;
	var iDiceSize = 6;
	
	var playerRoll1 = random(1,iDiceSize), playerRoll2 = random(1,iDiceSize);
	var mobRoll1 = random(1,iDiceSize), mobRoll2 = random(1,iDiceSize);
	
	//The mob gets a better chance of winning, of course.
	if( random(1,2) == 1 )
		mobRoll1 = Math.min(iDiceSize, mobRoll1+1);
	if( random(1,2) == 1 )
		mobRoll2 = Math.min(iDiceSize, mobRoll2+1);
	var playerTotal = playerRoll1 + playerRoll2;
	var mobTotal = mobRoll1 + mobRoll2;
	
	act("$n takes the coins from you and tosses them onto the table.", false, self, null, actor, constants.TO_VICT);
	act("You take the coins from $n and toss them onto the table.", false, self, null, actor, constants.TO_CHAR);
	act("$n takes the coins from $N and tosses them onto the table.", false, self, null, actor, constants.TO_NOTVICT);
	
	wait 2;
	
	act("$n grabs a bag from the back of the room and tosses it next to your coins on the table.", false, self, null, actor, constants.TO_VICT);
	act("You go to the back of room and grab a bag of coins. You throw them next to $N's coins on the table", false, self, null, actor, constants.TO_CHAR);
	act("$n grabs a bag from the back of the room and tosses it next to $N's coins on the table.", false, self, null, actor, constants.TO_NOTVICT);
	
	wait 4;
	
	self.say("You're up first!");
	
	wait 1;
	
	act("$n hands $N a cup containing two dice.", true, self, null, actor, constants.TO_NOTVICT);
	act("$n hands you a cup containing two dice.", false, self, null, actor, constants.TO_VICT);
	act("You hand $N a cup containing two dice.", false, self, null, actor, constants.TO_CHAR);
	
	wait 3;
	
	actor.send("You take a deep breath and toss the dice onto the table.");
	act("$n takes a deep breath and tosses the dice onto the table.", false, actor, null, null, constants.TO_ROOM);
	
	wait 3;
	
	here.echo("The dice land on " + playerRoll1 + " and " + playerRoll2 + ".");
	
	wait 3;
	
	if( playerTotal >= 10 )
		self.comm("cringe");
	else if( playerTotal >= 8 )
		self.comm("mutter");
	else if( playerTotal >= 6 )
		self.comm("boggle");
	else if( playerTotal >= 4 )
		self.comm("smirk");
	else {
		self.comm("laugh " + actor.name);
		wait 1;
		self.say("Any last words?");
	}
	wait 3;
	self.comm("emote scoops up the dice, rolls, and tosses them onto the table.");
	wait 4;
	here.echo("The dice land on " + mobRoll1 + " and " + mobRoll2 + ".");
	
	waitpulse 5;
	
	if( playerTotal > mobTotal ) {
		randomFreakout(self);
		waitpulse 5;
		randomFreakout(self);
		waitpulse 5;
		randomFreakout(self);
		
		wait 2;
		
		self.say("Bah! I guess you won fair and square!");
		
		var records = loadRecords(self);
		if( !records.overall || coppersToGamble > records.overall.amount ) {
			wait 3;
			
			if( records.overall )
				var lastRecord = " of " + records.overall.amount + ", achieved by " + records.overall.playerName + ".";
			else
				var lastRecord = ".";
			
			self.say("Congratulations! You just beat the previous overall record" + lastRecord);
			
			records.overall = new Object();
			records.overall.playerName = actor.name;
			records.overall.amount = coppersToGamble;
			records.overall.time = time();
			
			saveRecords(self,records);
		}
		
		actor.gold += coppersToGamble * 2;
	}
	else if( playerTotal == mobTotal ) {
		self.comm("roll");
		wait 2;
		self.say("A tie! I hate when that happens.");
		wait 2;
		act("$n gives you back your coins.", false, self, null, actor, constants.TO_VICT);
		act("You give $N back $S coins.", false, self, null, actor, constants.TO_CHAR);
		act("$n gives $N back $S coins.", true, self, null, actor, constants.TO_NOTVICT);
		actor.gold += coppersToGamble;
	}
	else {
		randomBrag(self);
		waitpulse 5;
		randomBrag(self);
		waitpulse 5;
		randomBrag(self);
		
		wait 3;
		
		self.say("I have a mortgage or two I could put that towards.");
		wait 1;
		self.comm("emote scoops the coins up from the table.");
	}
}
} finally {
	delete self.isInUse;
}



};

