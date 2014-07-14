var script8272 = function(self, actor, here, args, extra)
{
	//Alder
//CTF Leader Death Sets Other Side
function afterWait(args) {
	var leadVnum = args[0];
	var deathRoom = args[1];
	var vnum = deathRoom.vnum+"";
	if ( vnum[0] == 8 && vnum[1] == 2 ) {
		var room = getRoom(8271);
		switch (leadVnum) {
			case 3002:
				setSval(room,8271,"TV_PLAINS_CTF",2);
				mudLog(3,102,"DARK Lost Tar Valon Plains Fort");
				break;
			case 19513:
				setSval(room,8271,"TV_PLAINS_CTF",1);
				mudLog(3,102,"LIGHT Lost Tar Valon Plains Fort");
				break;
		}
	}
}
setTimeout(1,afterWait,[self.vnum,self.room]);

};

