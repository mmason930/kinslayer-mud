var script9858 = function(self, actor, here, args, extra)
{
		var word = getArgList(args)[0];
	var password = actor.getPval("ZIND_PASS");
	actor.detach(9858);
	if ( password == word && actor.affectedBy(constants.AFF_NOQUIT) == false ) {
		var playerCount = 0;
		for each ( var person in actor.room.people ) {
			if ( person.vnum == -1 )
				++playerCount;
		}
		for each ( var person in getRoom(9860).people ) {
			if ( person.vnum == 9860 ) {
				function checkQvals(element, elementIndex, targetArray) {
					if ( actor.quest(element.questName) == -1 ) {
						return ( element );
					}
				}
				var quests = actor.getAvailableQuests(9860);
				if ( quests.every(checkQvals) == true && actor.quest("A Floating Liaison") < 1 ) {
					waitpulse 7;
					actor.room.echo("A muffled voice from somewhere beyond the wall can be heard: 'There's nothing for you right now.'");
					return;
				}
			}
			if ( person.vnum == -1 ) {
				waitpulse 7;
				actor.room.echo("A muffled voice from somewhere beyond the wall can be heard: 'Come back later.'");
				return;
			}
		}
		waitpulse 7;
		if ( playerCount == 1 ) {
			getCharCols(actor,constants.CL_OFF);
			actor.room.echo("You hear some rustling behind the south wall of the hold and the hatch swings up.");
			waitpulse 14;
			actor.send("You crouch down and enter the hatchway.");
			actor.room.echoaround(actor, capFirstLetter(actor.name)+" crouches down and enters the hatchway.");
			waitpulse 3;
			actor.room.echoaround(actor, "The hatch quickly slams shut behind "+actor.name+".");
			actor.moveToRoom(getRoom(9860));
			actor.comm("look");
			if ( actor.quest("A Floating Liaison") == 1 ) {
				for each ( var person in actor.room.people ) {
					if ( person.vnum == 9860 ) {
						var zind = person;
					}
				}
				waitpulse 16;
				zind.comm("say Welcome to my abode, "+actor.name+". I am Zind Ambla. I trust your visit by Ourd was pleasant enough...or was it Noin...Bah there are so many it hardly matters!");
				waitpulse 28;
				actor.send(capFirstLetter(zind.name)+" releases a satisfied chortle.");
				waitpulse 10;
				zind.comm("say You have gained entry into this fine operation we have here, and now you will work for us. Consider this a payment for expected fealty.");
				waitpulse 24;
				actor.updateJournalTask("A Floating Liaison",0);
				waitpulse 24;
				zind.comm("say For now, you'll get jobs directly from me. You can"+cyn+" leave "+nrm+"this room whenever you want to.");
				waitpulse 7;
				actor.send("Use the "+cyn+"quest list"+nrm+" command to view jobs for Zind, and the "+cyn+"quest"+nrm+" command to view general commands available at quest masters.");
			}
		}
		else {
			actor.room.echo("A muffled voice from somewhere beyond the wall can be heard: 'Lose the company.'");
		}
		
	}
	

};

