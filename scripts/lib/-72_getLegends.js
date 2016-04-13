//Alder
//May 2010
/******************************************************************
 * Returns array of Legends names in order starting with Legend 1 *
 ******************************************************************/
function getLegends() {
	var rs = sqlQuery("SELECT username FROM `users` WHERE level<100 AND !(plr & (1<<22)) ORDER BY `users`.`weave` DESC LIMIT 0 , 8;");
	var legends = [];
	while ( rs.hasNextRow ) {
		var row = rs.getRow;
		legends.push(row.get("username"));
	}
	return legends;
}
