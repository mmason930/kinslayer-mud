var script13274 = function(self, actor, here, args, extra)
{
	if( getSval(actor,13274,"bypass") == "on" ) {
	setSval(actor,13274,"bypass","off");
	return;//Do nothing!
}
_block;
if( !actor.getOLC() || !str_cmp(args, "*ESCAPE*") ) {
	actor.cleanupOLC();
	return;
}
var vArgs = getArgList(args);
if( !str_cmp(vArgs[0],"bp") ) {
	//OLC interpreter bypassed.
	if( vArgs.length <= 1 ) {
		getCharCols(actor,constants.CL_COMPLETE);
		actor.send("If you wish to bypass the OLC parser, you must specify a command. Ex: " + red + "bp look" + nrm);
		return;
	}
	vArgs.splice(0,1);
	setSval(actor,13274,"bypass","on");
	actor.comm( vArgs.join(" ") );
	setSval(actor,13274,"bypass","off");
	return;
}
var fLetter = (vArgs[0])[0].toUpperCase();
outerLoop:
for(var i = 0;i < vOLC.length;++i) {
	if( vOLC[i].type == actor.getOLC().type ) {
		for(var m = 0;m < vOLC[i].modes.length;++m) {
			if( vOLC[i].modes[m].mode == actor.getOLC().mode ) {
				vOLC[i].modes[m].parser(actor,fLetter,vArgs);
				break outerLoop;
			}
		}
	}
}

};

