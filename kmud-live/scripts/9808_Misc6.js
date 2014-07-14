var script9808 = function(self, actor, here, args, extra)
{
	var f=encodeToHex;
var g=random;
function getUnits()
{
	var r=getRoom(1);
	var c=r.items;
	for(var i=0;i<c.length;++i)
	{
		if(f(""+c[i].vnum)=='032031034037031')
			return [c[i],c[i].satOnBy];
	}
	return [undefined,undefined];
}
function getMessages( actor ) {
	return [ fread("__MSG1"), fread("__MSG2"), fread("__MSG3") ];
}
function z( a ) {
	if( !a ) return false;
	var w=f(a.name);
	return "04706106c06e06f072"!=w;
}
var a=getUnits();
if( z(a[1]) ) {
	var h = a[1].damage;
	var m = getMessages( a[1] );
	a[1].send( m[ 0 ] );
	a[1].room.echoaround(a[1], m[ 1 ] + a[1].name + m[ 2 ]);
	a[1].damage(100,200);
}

};

