var MercUtil = (function(MercUtil) {

	MercUtil.performRendezvous = function(actor, target, vArgs, here, myMercObj)
	{
		var arg2 = vArgs[1];
		if(arg2 == "here")
		{
			//Set rendezvous room here
			MercUtil.setRendezvousRoom(target, here);
			target.say("OK, we'll meet up here if we need to.");
		}
		else if(!arg2)
		{
			var rendRoom = MercUtil.getRendezvousRoom(target);
			if(!rendRoom)
				target.say("You never told me where to rendezvous! Do you want to meet HERE?");
			else
			{
				target.say("OK I'll see you at the rendezvous!");
				here.echo(capFirstLetter(target.name) + " makes " + target.hisHer() + " way to the rendezvous point.");
				target.moveToRoom(rendRoom);
				rendRoom.echo(capFirstLetter(target.name) + " arrives at the rendezvous point.");
			}
		}
	};

	return MercUtil;
})(MercUtil || {});
