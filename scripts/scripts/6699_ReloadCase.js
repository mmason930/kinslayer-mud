var script6699 = function(self, actor, here, args, extra) {
	//Alder
	//August 2010
	var items = self.items;
	for (var _autoKey in items) {
		var item = items[_autoKey];
		if ( item.vnum == 6698 ) {
			var vCase = item;
		}
	}
	if ( vCase ) {
		vCase.extract();
	}
	self.loadObj(6698);
}