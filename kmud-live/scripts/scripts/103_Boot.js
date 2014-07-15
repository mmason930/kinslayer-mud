var script103 = function(self, actor, here, args, extra) {
	if( actor.level < 104 ) {
		return;
	}
	_block;
	var vArgs = getArgList( args );
	function getBootMode()
	{
		var m = fread("bootMode");
		if( !m || m.length == 0 ) {
			m = "NORMAL";
			setBootMode( "NORMAL" );
		}
		return m;
	}
	function setBootMode( m )
	{
		if( !m || m.length == 0 )
			m = "NORMAL";
		fwrite("bootMode",m);
	}
	if( vArgs.length < 2 )
	{
		actor.send("Boot mode currently set to: " + getBootMode());
	}
	else
	{
		var mode = vArgs[1].toUpperCase();
		if( mode != "NORMAL" && mode != "VALGRIND" ) {
			here.echo("Boot mode can be set to: NORMAL, VALGRIND");
			return;
		}
		setBootMode( mode );
		actor.send("Boot mode set to: " + mode);
		mudLog(constants.BRF, Math.max(actor.invis,102), "Boot mode set to " + mode + " by " + actor.name + ".");
	}
	
}