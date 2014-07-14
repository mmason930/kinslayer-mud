var script9886 = function(self, actor, here, args, extra)
{
	//Alder
//May 2010
var i;
if ( self.name == "a chunk of gold ore" ) {
	i = 0;
}
else if ( self.name == "a chunk of platinum ore" ) {
	i = 1;
}
else if ( self.vnum == 1002 )
	i = 2;
actor.updateJournalTask('Precious Metals',i);

};

