var MercUtil = (function(MercUtil) {

	MercUtil.performCrawl = function(actor, target, vArgs, here, myMercObj)
	{
		if(vArgs[1] != "out" && vArgs[1] != "entrance")
			target.say("Crawl? What am I crawling around in?!");
		else
			target.comm("crawl " + vArgs[1]);
	};

	return MercUtil;
})(MercUtil || {});
