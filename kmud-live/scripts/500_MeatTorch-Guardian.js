var script500 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
if( !str_cmp(vArgs[0],"meat") || !str_cmp(vArgs[0],"food") ) {
	self.loadObj(2104);
	self.comm("give meat " + actor.name);
}
else if( !str_cmp(vArgs[0],"light") || !str_cmp(vArgs[0],"torch") ) {
	self.loadObj(2040);
	self.comm("give light " + actor.name);
}

};

