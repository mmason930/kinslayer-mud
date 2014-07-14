var script10645 = function(self, actor, here, args, extra)
{
	_block;
var arg_array = getArgList(args);
var text = arg_array.splice(0);
text = text.join(" ");
var people = actor.room.people;
   for each(var person in people)
      {
         if (person != actor) {
            person.send(capFirstLetter(actor.name) + " drawls, " + strColFormat((text),80) + "'");
      }
   }
   actor.send("You drawl, " + text + "'");

};

