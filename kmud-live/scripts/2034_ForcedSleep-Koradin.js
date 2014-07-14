var script2034 = function(self, actor, here, args, extra)
{
	var sval = getSval(actor,2034,"sleeping");
if (!sval)
	sval = 0;
if (sval < time()) {
	actor.detach(2034);
	return;
}
actor.send("You can only dream about doing that...");
_block;
return;


};

