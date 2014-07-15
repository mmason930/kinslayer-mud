var script13299 = function(self, actor, here, args, extra) {
	for( var i = 0;i < global.vQuests.length;++i ) {
		if( vQuests[i].ownerVnum == self.vnum && vQuests[i].ownerRoomVnum == self.room.vnum )
			vQuests[i].owner = self;
	}
}