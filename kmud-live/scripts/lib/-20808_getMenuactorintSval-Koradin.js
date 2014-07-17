//getMenu(actor,intSval) returns a menu message to the actor for receiving menus during smithing.
//Koradin - May 2010
function getMenu( actor, intSval, strSval )
{
	if (strSval == "smithing")
	{
		getCharCols(actor);
		//Smithing a WEAPON below
		if (intSval == 1)
		{
			actor.send(" ");
			actor.send("Will the weapon be one-handed or two-handed?");
			actor.send(" ");
			actor.send("["+cyn+"1"+nrm+"] One-Handed    ["+cyn+"2"+nrm+"] Two-Handed    ["+cyn+"C"+nrm+"] Cancel");
			actor.send(" ");
			return;
		}
		else if (intSval == 2)
		{
			actor.send(" ");
			actor.send("Will the weapon be more offensive, or more defensive?");
			actor.send(" ");
			actor.send("["+cyn+"O"+nrm+"] Offensive     ["+cyn+"D"+nrm+"] Defensive    ["+cyn+"B"+nrm+"] Go Back");
			actor.send("["+cyn+"C"+nrm+"] Cancel");
			actor.send(" ");
			return;
		}
		else if (intSval == 3)
		{
			actor.send(" ");
			actor.send("Will the weapon be a throwing spear, or throwing axe?");
			actor.send(" ");
			actor.send("["+cyn+"S"+nrm+"] Spear      ["+cyn+"A"+nrm+"] Axe      ["+cyn+"C"+nrm+"] Cancel");
			actor.send(" ");
			return;
		}
		else if (intSval == 4)
		{
			actor.send(" ");
			actor.send("Which attribute would you like to focus on?");
			actor.send(" ");
			actor.send("["+cyn+"H"+nrm+"] Hit Rate      ["+cyn+"D"+nrm+"] Damage      ["+cyn+"C"+nrm+"] Cancel");
			actor.send(" ");
			return;
		}
		else if (intSval == 5)
		{
			actor.send(" ");
			actor.send("Which attribute would you like to focus on?");
			actor.send(" ");
			actor.send("["+cyn+"D"+nrm+"] Dodge      ["+cyn+"P"+nrm+"] Parry      ["+cyn+"C"+nrm+"] Cancel");
			actor.send(" ");
			return;
		}
		else
			return;
	}
	else if (strSval == "tailoring") {
		if (intSval == 11) {  // COMBO
			actor.send(" ");
			actor.send("Do you want this armor to be heavier or lighter?");
			actor.send(" ");
			actor.send("["+cyn+"H"+nrm+"] Heavier      ["+cyn+"L"+nrm+"] Lighter      ["+cyn+"C"+nrm+"] Cancel");
			actor.send(" ");
			return;
		}
		else
			return;
	}
	else if (strSval == "mixing");
	{
		if (intSval == 1)
	{
		actor.send(" ");
		actor.send("You can currently purchase recipes for the following items:");
		actor.send(" ");
		actor.send("["+cyn+"1"+nrm+"] One-Handed    ["+cyn+"2"+nrm+"] Two-Handed    ["+cyn+"C"+nrm+"] Cancel");
		actor.send(" ");
		return;
	}
	}
}

