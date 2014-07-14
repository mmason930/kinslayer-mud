JSCharacter.prototype.__defineGetter__("eqList", function()
{
	var eq = [];
	for(var i = 0;i < constants.NUM_WEARS;++i) {
		if( !this.eq( i ) ) continue;
		eq.push( this.eq( i ) );
	}
	return eq;
}
);
