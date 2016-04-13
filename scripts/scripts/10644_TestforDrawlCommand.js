var script10644 = function(self, actor, here, args, extra) {
	_block;
	var arg_array = getArgList(args);
	var text = arg_array.splice(1);
	text = text.join(" ");
	var people = actor.room.people;
	   for (var _autoKey in people) {
	   	var person = people[_autoKey];
	         if (person != actor) {
	            person.send(capFirstLetter(actor.name) + " drawls, '" + strColFormat((text),80) + "'");
	      }
	   }
	   actor.send("You drawl, '" + text + "'");
}