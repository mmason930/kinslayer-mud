var script39 = function(self, actor, here, args, extra)
{
	_block;
var rs = sqlQuery("SELECT * FROM users WHERE level < 51 ORDER BY feat_points DESC LIMIT 0, 9;");
i = 1;
while ( rs.hasNextRow ) {
	var row = rs.getRow;
	var person = row.get("username");
	var fp = row.get("feat_points");
	actor.send((i++)+") "+person+": "+fp);
}

};

