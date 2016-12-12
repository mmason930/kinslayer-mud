var script107 = function(self, actor, here, args, extra) {
	if( actor.level < 103 ) {
		return;
	}
	_block;
	function getGold( m ) {
		return parseInt((m/100/100));
	}
	function getSilver( m ) {
		return parseInt((m - getGold(m)*10000)/100);
	}
	function getCopper( m ) {
		return parseInt(((m - getGold(m)*10000)-getSilver(m)*100));
	}
	function moneyString(actor,m) {
		getCharCols(actor);
		var sBuffer = bld + yel + strPadding((getGold(m)+""), " ", 5, "right") + nrm;
		sBuffer += " ";
		sBuffer += bld + cyn + strPadding((getSilver(m)+""), " ", 5, "right") + nrm;
		sBuffer += " ";
		sBuffer += yel + strPadding((getCopper(m)+""), " ", 5, "right") + nrm;
		return sBuffer;
	}
	var rs = sqlQuery("SELECT (gold+bank_gold) AS money,username FROM users WHERE level < 100 ORDER BY money DESC LIMIT 100;");
	while( rs.hasNextRow ) {
		var row = rs.getRow;
		var money = parseInt(row.get(money));
		actor.send( strPadding(row.get("username"), " ", 20, "right") + " " + moneyString(actor,money) );
	}
	actor.send("");
	rs = sqlQuery("SELECT SUM( gold+bank_gold ) as totalMoney FROM users WHERE level < 100;");
	var row = rs.getRow;
	var totalMoney = row.get("totalMoney");
	actor.send("Total gold in play: " + moneyString(actor,parseInt(totalMoney)));
}