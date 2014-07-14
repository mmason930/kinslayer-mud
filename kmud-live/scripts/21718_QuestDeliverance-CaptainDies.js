var script21718 = function(self, actor, here, args, extra)
{
	//Alder
//August 2010
//Quest: Deliverance
/** Death: a Captain of the Queen's Guard **/
var qName =  "Deliverance";
var pointMan = getSval(self,386,"DELIVERANCE_POINTMAN");
function afterWait(args) {
	pointMan = args[0];
	if ( pointMan ) {
		if ( pointMan.quest(qName) && pointMan.questTaskProgress(qName)[3].comp ) {
			pointMan.updateJournalTask(qName,3);
		}
	}
}
setTimeout(1,afterWait,[pointMan]);
	

};

