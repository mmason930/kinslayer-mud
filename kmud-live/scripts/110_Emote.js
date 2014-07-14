var script110 = function(self, actor, here, args, extra)
{
	var vArgs = args.split(" ");
_block;
if( vArgs.length <= 1 ) {
	actor.send("What do you want to emote?");
	return;
}
var people = here.people;
vArgs.splice(0,1);
var sEmote = vArgs.join(" ");
for(var personIndex = 0;personIndex < people.length;++personIndex) {
	if( people[ personIndex ].race != actor.race && actor.level < 100 && people[ personIndex ].level < 100 )
		act("$N makes a strange gesture.", true, people[ personIndex ], null, actor, constants.TO_CHAR);
	else
		act("$N " + sEmote, true, people[ personIndex ], null,actor, constants.TO_CHAR);
}

};

