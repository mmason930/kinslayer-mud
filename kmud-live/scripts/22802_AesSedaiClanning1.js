var script22802 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
if( vArgs.length >= 7 ) {
	if( !str_cmp(vArgs[0],"please") && !str_cmp(vArgs[1],"bring") && !str_cmp(vArgs[3],"in") && !str_cmp(vArgs[4],"for") && !str_cmp(vArgs[5],"her") && (!str_cmp(vArgs[6],"test") || !str_cmp(vArgs[6],"test.")) )
	{
		var name = vArgs[2];
		if( !actor.isAesSedai() )
			return;
		self.comm("narrate " + capFirstLetter(name.toLowerCase()) + ", you are summoned to be tested for the shawl of an Aes Sedai. The Light keep you whole and see you safe.");
	}
}

};

