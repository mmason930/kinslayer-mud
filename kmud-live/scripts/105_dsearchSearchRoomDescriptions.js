var script105 = function(self, actor, here, args, extra)
{
	if( actor.level < 100 ) {
	return;
}
_block;
var vArgs = getArgList( args );
if( vArgs.length < 2 ) {
	actor.send("dsearch will search all rooms for a chunk of text in the room description.\r\nYou must specify the chunk of text to search for.");
	return;
}
vArgs.reverse();
vArgs.pop();
vArgs.reverse();
var vIncludes = [];
var vExcludes = [];
for(var i = 0;i < vArgs.length;++i) {
	var str = vArgs[i];
	if( str.charAt(0) == "+" )
		vIncludes.push( str.substr(1).toLowerCase() );
	else if( str.charAt(0) == "-" )
		vExcludes.push( str.substr(1).toLowerCase() );
	else
		continue;
	vArgs.splice(i,1);
	--i;
}
args = vArgs.join(" ");
args = args.toLowerCase();
getCharCols(actor,constants.CL_NORMAL);
actor.send("The following rooms contain the raw text '" + bld + cyn + args + nrm + "' :");
actor.send("Includes the following words: " + bld + grn + vIncludes.join(" ") + nrm);
actor.send("Excludes the following words: " + bld + yel + vExcludes.join(" ") + nrm);
actor.send("------------------------------------------------------------------------------");
var i = 0;
var matches = 0;
var buffer = "";
mainLoop:
for(var room = getRoomByRnum(i);room = getRoomByRnum(i);++i)
{
	var sDescription = room.description.toLowerCase();
	for(var j = 0;j < vIncludes.length;++j) {
		var patt = "\\W?" + vIncludes[j] + "\\W?";
		var match = sDescription.search( patt );
		if( match == -1 ) {
			continue mainLoop;
		}
	}
	for(var j = 0;j < vExcludes.length;++j) {
		var patt = "\\W?" + vExcludes[j] + "\\W?";
		var match = sDescription.search( patt );
		if( match != -1 ) {
			continue mainLoop;
		}
	}
	if( args.length == 0 || sDescription.toLowerCase().indexOf( args ) != -1 ) {
		if( matches != 0 ) {
			buffer += ", ";
			if( (matches % 5) == 0 )
				buffer += "\n";
		}
		buffer += strPadding(room.vnum+""," ",6,"left");
		++matches;
	}
}
actor.send("");
if( buffer.length > 0 ) {
	actor.send(buffer);
} else {
	actor.send("No rooms matched your search.");
}
	
	

};

