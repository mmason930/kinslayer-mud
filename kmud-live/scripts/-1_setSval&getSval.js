function setSval(o, vnum, name, value)
{
	var id = "s" + vnum + "_" + name;
	o[id] = value;
	return value;
}
function getSval(o, vnum, name)
{
	var id = "s" + vnum + "_" + name;
	if (o[id] == undefined)
	{
//		o[id] = 0;
//		return 0;
		return undefined;
	}
	else
		return o[id];
}
