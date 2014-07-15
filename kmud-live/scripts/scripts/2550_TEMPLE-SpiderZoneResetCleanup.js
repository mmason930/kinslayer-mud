var script2550 = function(self, actor, here, args, extra) {
	here.getObjects(TempleManager.spiderLeverObjectVnum).forEach(function(item) {
		item.extract();
	});
}