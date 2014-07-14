var script10667 = function(self, actor, here, args, extra)
{
	var lead = self.leader;
if ( lead ) {
	var recruits = getSval(lead, 10660, "greyman_recruit");
	var remainder = 5 - recruits;
	getCharCols(lead);
	lead.send(bld+cyn+"You need " + remainder + " more recruits."+nrm);
	recruits -= 1;
	setSval(lead, 10660, "greyman_recruit", recruits);   
}

};

