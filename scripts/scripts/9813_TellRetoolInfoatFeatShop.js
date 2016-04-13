var script9813 = function(self, actor, here, args, extra) {
	// Alder
	// May 2011
	/** Set retool info for feat point shop **/
	if ( !actor.featRetool ) {
		return;
	}
	var vArgs = getArgList(args);
	var attribute = vArgs.shift();
	here.echo(attribute);
	var info = vArgs.join(" ");
	var name = !str_cmp(attribute,"name");
	var aliases = !str_cmp(attribute,"aliases");
	var roomDesc = !str_cmp(attribute,"roomDesc");
	if ( name ) {
		actor.featRetool.name = info;
		self.tell(actor, "Retool name set to: \""+info+"\"");
	}
	if ( aliases ) {
		actor.featRetool.aliases = info;
		self.tell(actor, "Retool aliases set to: \""+info+"\"");
	}
	if ( roomDesc ) {
		actor.featRetool.roomDesc = info;
		self.tell(actor, "Retool roomDesc set to: \""+info+"\"");
	}
	if ( actor.featRetool.name.length && actor.featRetool.aliases.length && actor.featRetool.roomDesc.length ) {
		actor.featRetool.complete = true;
		actor.send("");
		self.tell(actor, "You've given me information for all retool fields. Here's what I have:");
		self.tell(actor, "Name: \""+actor.featRetool.name+"\"");
		self.tell(actor, "Aliases: \""+actor.featRetool.aliases+"\"");
		self.tell(actor, "roomDesc: \""+actor.featRetool.roomDesc+"\"");
		self.tell(actor, "You can now hand in your item for retooling, provided you still possess enough feat points.");
	}
}