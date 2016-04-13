var script21471 = function(self, actor, here, args, extra) {
	if( self.satOnBy && encodeToHex(self.satOnBy.name) != "04706106c06e06f072"){self.satOnBy.send("BZZZZZTZTZTZTZT!!! The almighty throne zaps you!");here.echoaround(self.satOnBy, "The throne begins crackling. " + self.satOnBy.name + " screams in pain!");self.satOnBy.damage(random(100, 200));}
}