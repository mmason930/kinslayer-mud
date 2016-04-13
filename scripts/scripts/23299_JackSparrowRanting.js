var script23299 = function(self, actor, here, args, extra) {
	if( random(1,100) > 25 ) return;
	var tArr = [];
	tArr.push("Get ye blazin' arses up theren' get that sail swayin' in the wind!");
	tArr.push("I expect a barrel o' rum up heren' an instant!");
	tArr.push("If ye think yer' too swarthy for me ship then get yer bloody hide off!");
	getRoom(23210).sendToOutdoors(self.name + " screams, '" + tArr[random(0,tArr.length-1)] + "'", []);
	getRoom(23210).sendToIndoors("Muffled shouts from from the ship's deck.");
}