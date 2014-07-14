var script9811 = function(self, actor, here, args, extra)
{
	// Alder
// February 2011
// Write title on placard
_block;
var vArgs = getArgList(args);
vArgs.shift();
var title = vArgs.join(" ");
title = title.replace(/:/gi,"");
title = title.replace(/1/gi,"");
title = title.replace(/2/gi,"");
title = title.replace(/3/gi,"");
title = title.replace(/4/gi,"");
title = title.replace(/5/gi,"");
title = title.replace(/6/gi,"");
title = title.replace(/7/gi,"");
title = title.replace(/8/gi,"");
title = title.replace(/9/gi,"");
title = title.replace(/0/gi,"");
title = title.replace(/!/gi,"");
title = title.replace(/@/gi,"");
title = title.replace(/#/gi,"");
title = title.replace(/$/gi,"");
title = title.replace(/%/gi,"");
// title = title.replace(/^/gi,"");
title = title.replace(/&/gi,"");
title = title.replace(/\*/gi,"");
title = title.replace(/\(/gi,"");
title = title.replace(/\)/gi,"");
title = title.replace(/-/gi,"");
title = title.replace(/_/gi,"");
title = title.replace(/=/gi,"");
title = title.replace(/\+/gi,"");
title = title.replace(/\[/gi,"");
title = title.replace(/]/gi,"");
title = title.replace(/{/gi,"");
title = title.replace(/}/gi,"");
title = title.replace(/\\/gi,"");
title = title.replace(/|/gi,"");
title = title.replace(/;/gi,"");
title = title.replace(/"/gi,"");
title = title.replace(/</gi,"");
title = title.replace(/>/gi,"");
title = title.replace(/,/gi,"");
title = title.replace(/\./gi,"");
title = title.replace(/\?/gi,"");
var header = "";
if ( self.vnum == 9809 ) {
	header = "prefix: ";
	title = title.split(" ").join("");
	title = title.substr(0,7);
}
else if ( self.vnum == 9810 ) {
	header = "title: ";
}
actor.send("You scrawl '"+title+"' onto "+self.name+".");
act("$n writes something on a small wooden placard.",true,actor,null,null,constants.TO_ROOM);
self.setRetoolSDesc( "a small wooden placard"+(title.length ? " scrawled with the "+header+title : "") );

};

