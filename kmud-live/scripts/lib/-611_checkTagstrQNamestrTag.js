//Alder
//April 2010
/************************************************************************
 * strQName: name of quest whose tags are being examined                *
 * strTag: string which is compared with objQuest's tags                *
 * Returns: true if strTag is a tag of quest named strQName, else false *
 ************************************************************************/
	function checkTag ( strQName, strTag ) {
		var objQuest = getQuestByName(strQName);
		var strTag = strTag+"";
		for each ( var tag in objQuest.tags ) {
			if ( tag == strTag ) {
				return true;
			}
		}
		return false;
	}
