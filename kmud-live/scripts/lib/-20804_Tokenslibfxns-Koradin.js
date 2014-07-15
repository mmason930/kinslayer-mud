//TOKENS Library Functions
//Koradin - May 2010
function getAllTokens()
{
	var tokens = [];
	//LIST NEW TOKEN TYPES HERE
	var honor = ["Honor", "           "];
	var resol = ["Resolve", "         "];
	var power = ["Power", "           "];
	var enhan = ["Enhancement", "     "];
	var skill = ["Skill", "           "];
	var cleve = ["Cleverness", "      "];
	var death = ["Death", "           "];
	//THEN PUSH THEM INTO THE TOKEN ARRAY
	tokens.push(honor);
	tokens.push(resol);
	tokens.push(power);
	tokens.push(enhan);
	tokens.push(skill);
	tokens.push(cleve);
	tokens.push(death);
	
	//
	return tokens;
}
function getTokens( actor, strTokenType )
{
	for each ( var token in getAllTokens() )
	{
		if (strTokenType == token[0])
		{
			if (actor.quest("TOKENS_"+token[0]) > 0 )
				return actor.quest("TOKENS_"+token[0]);
		}
	}
	return 0;
}
function updateTokens( actor, strTokenType, number )
{
	getCharCols(actor);
	var trigger = false;
	for each (var token in getAllTokens())
	{
		if (token[0] == strTokenType)
			trigger = true;
	}
	if (trigger == false)
		return;
	if (actor.quest("TOKENS_"+strTokenType) > 0)
		actor.qval("TOKENS_"+strTokenType, actor.quest("TOKENS_"+strTokenType)+number);
	else
		actor.qval("TOKENS_"+strTokenType, number);
	if (number < 1)
		return;
	// else if (number == -1)
	// {
		// var gL = "lost ";
		// var tok = " token";
	// }
	// else if (number < -1)
	// {
		// var gL = "lost ";
		// var tok = " tokens";
	// }
	else if (number == 1)
	{
		var gL = "earned ";
		var tok = " token";
	}
	else if (number > 1)
	{
		var gL = "earned ";
		var tok = " tokens";
	}
	actor.send(mag+bld+"You have "+gL+Math.abs(number)+" "+strTokenType+tok+"!"+nrm);
}
