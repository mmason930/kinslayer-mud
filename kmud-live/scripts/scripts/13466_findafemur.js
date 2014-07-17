var script13466 = function(self, actor, here, args, extra) {
	if( actor.room != here )
	{
	        _block;
	        actor.send("You stop digging.");
	        return;
	}
	here.echoaround(actor, actor.name + " kneels down before a mound of earth.");
	actor.send("You kneel down before a mound of earth.");
	waitpulse 20;
	if( actor.room != here )
	{
	        _block;
	        actor.send("You stop digging.");
	        return;
	}
	here.echoaround(actor, actor.name + " brushes some dead flowers out of the way.");
	actor.send("You gently move some dead flowers out of the way.");
	waitpulse 20;
	if( actor.room != here )
	{
	        _block;
	        actor.send("You stop digging.");
	        return;
	}
	here.echoaround(actor, actor.name + " starts to dig.");
	actor.send("You plunge your hands into the moist earth and begin to dig.");
	waitpulse 18;
	if( actor.room != here )
	{
	        _block;
	        actor.send("You stop digging.");
	        return;
	}
	here.echoaround(actor, actor.name + " digs.");
	actor.send("You dig...");
	waitpulse 18;
	if( actor.room != here )
	{
	        _block;
	        actor.send("You stop digging.");
	        return;
	}
	here.echo("A small earthworm slithers out from some dug up earth.");
	waitpulse 18;
	if( actor.room != here )
	{
	        _block;
	        actor.send("You stop digging.");
	        return;
	}
	here.echoaround(actor, capFirstLetter(actor.name) + " removes some dirt from beneath " + his_her(actor.sex) + " fingernails.");
	actor.send("You pick some dirt from underneath your fingernails.");
	waitpulse 18;
	if( actor.room != here )
	{
	        _block;
	        actor.send("You stop digging.");
	        return;
	}
	here.echoaround(actor, actor.name + " keeps digging.");
	actor.send("You keep digging....");
	waitpulse 18;
	if( actor.room != here )
	{
	        _block;
	        actor.send("You stop digging.");
	        return;
	}
	actor.send("Your hands feel cool as they dig deeper into the earth.");
	waitpulse 18;
	if( actor.room != here )
	{
	        _block;
	        actor.send("You stop digging.");
	        return;
	}
	here.echoaround(actor, capFirstLetter(actor.name) + " ajusts " + his_her(actor.sex) + " position slightly and continues digging.");
	actor.send("You shift about, trying to get comfortable.");
	waitpulse 18;
	if( actor.room != here )
	{
	        _block;
	        actor.send("You stop digging.");
	        return;
	}
	actor.send("Your hand hits something hard.");
	waitpulse 18;
	if( actor.room != here )
	{
	        _block;
	        actor.send("You stop digging.");
	        return;
	}
	here.echoaround(actor, actor.name + " extracts a long slender bone from the earth.");
	actor.send("You remove a slender human bone from the earth.");
	actor.loadObj(13412);
}