JSCharacter.prototype.__defineGetter__("totalQuestPoints",
function()
{
	var vClans = this.findClans(true);
	if( vClans.length == 0 ) return 0;
	
	var totalQP = 0;
	for(var i = 0;i < vClans.length;++i) {
		totalQP += this.qp( vClans[ i ] );
	}
	return totalQP;
}
);
