var script6447 = function(self, actor, here, args, extra)
{
		if ( actor.quest("Drop Dead") > 2 ) {
		function f(arg) {
			var actor = arg[0];
			actor.qval("Drop Dead",10);
			actor.comm("save");
			getCharCols(actor);
			actor.send(cyn+bld+"That man looked very familiar..."+nrm);
		}
		setTimeout(2,f,[actor]);
	}

};

