var script22814 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	if( vArgs.length >= 8 ) {
	        if( !str_cmp(vArgs[0],"please") && !str_cmp(vArgs[1],"bring") && !str_cmp(vArgs[3],"in") && !str_cmp(vArgs[4],"for") && !str_cmp(vArgs[5],"her") && !str_cmp(vArgs[6],"accepted") && (!str_cmp(vArgs[7],"test") || !str_cmp(vArgs[7],"test.")) )
	        {
	                var name = vArgs[2];
	                if( !actor.isAesSedai() )
	                        return;
	                self.comm("narrate " + capFirstLetter(name.toLowerCase()) + ", you are summoned to the Hall of the White Tower. The Light keep you whole and see you safe.");
	        }
	}
}