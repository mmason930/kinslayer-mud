//Alder
//April 2010
/************************************************************************
 * strQName: name of quest whose tags are being examined                *
 * strTag: string which is compared with objQuest's tags                *
 * Returns: true if strTag is a tag of quest named strQName, else false *
 ************************************************************************/
	function checkTag ( strQName, strTag ) {
		var objQuest = getQuestByName(strQName);
		strTag = strTag+"";
		for (var _autoKey in objQuest.tags) {
			var tag = objQuest.tags[_autoKey];
			if ( tag == strTag ) {
				return true;
			}
		}
		return false;
	}

