var script23264 = function(self, actor, here, args, extra)
{
	var args = args.split(" ");
if ( args[0] == "disp" ) {here.echo("Loaded: " + getSval(self, 23263, "loaded"));}
else if ( args[0] == "reset" ) {
   setSval(self, 23263, "loaded", 0);
   here.echo("Loaded: " + getSval(self, 23263, "loaded"));
   }

};

