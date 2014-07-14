var script10640 = function(self, actor, here, args, extra)
{
	///Jenkin 
//September 2010
//Quest: Shienaran Expedition
var qName = 'The Shienaran Expedition';
if ( actor.questInProgress(qName) ) {
self.moveToRoom(getRoom(501));
self.extract();
}

};

