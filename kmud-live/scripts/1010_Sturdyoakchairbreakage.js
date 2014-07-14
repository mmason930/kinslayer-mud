var script1010 = function(self, actor, here, args, extra)
{
	if(random(1,5)== 1 ){
	if (actor.position = constants.POS_STANDING)
	{
		here.echoaround(actor, "A sturdy oak chair breaks under the strain as it is sat upon!");
		actor.damage(5);
		actor.send("A sturdy oak chair breaks under the weight of your body! Hopefully it was just a structural flaw.");
		here.echo("A replacement chair is called for and is quickly brought in.");
	_block;
	}
}

};

