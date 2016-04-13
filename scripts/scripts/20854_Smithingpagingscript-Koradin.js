var script20854 = function(self, actor, here, args, extra) {
	//Smithing paging script
	//Koradin - May 2010
	_block;
	getCharCols(actor);
	var sval = getSval(actor,20851,"paging");
	var weapon_type = getSval(actor,20851,"weapon type");
	var weapon_mat = getSval(actor,20851,"weapon mat");
	var vArgs = getArgList(args);
	if (strn_cmp(vArgs[0],"cancel",1))
	{
		actor.send("You stop smithing.");
		setSval(actor,20851,"paging",0);
		setSval(actor,20851,"weapon type",null);
		setSval(actor,20851,"handed",0);
		setSval(actor,20851,"attribute",0);
		actor.detach(20854);
		return;
	}
	//Single digit svals For WEAPONS
	if (sval == 1)
	{
		if (vArgs[0] == 1 || strn_cmp(vArgs[0],"one",1))
		{
			setSval(actor,20851,"handed",1);
			if (weapon_type == "Axe" || weapon_type == "Spear" || weapon_type == "Lance" || weapon_type == "Chain") {
				setSval(actor,20851,"attribute",2);
				actor.detach(20854);
				actor.comm("smith "+weapon_mat+" "+weapon_type);
			}
			else {
				setSval(actor,20851,"paging",2);
				getMenu(actor,2,"smithing");
			}
			return;
		}
		else if (vArgs[0] == 2)
		{
			if (weapon_type != "Small Projectile" && weapon_type != "Large Projectile" && weapon_type != "Short Blade")
			{
				setSval(actor,20851,"handed",2);
				if (weapon_type == "Chain" || weapon_type == "Axe" || weapon_type == "Club") {
					setSval(actor,20851,"attribute",1);
					actor.detach(20854);
					actor.comm("smith "+weapon_mat+" "+weapon_type);
				}
				else {
					setSval(actor,20851,"paging",2);
					getMenu(actor,2,"smithing");
				}
				return;
			}
			else
			{
				actor.send(" ");
				actor.send(red+"The weapon you're trying to smith can only be one-handed."+nrm)
				getMenu(actor,1,"smithing");
				return;
			}
		}
		else
		{
			actor.send("That is not a valid command!");
	        actor.send("Choose another command, or type "+cyn+"C"+nrm+" to stop smithing.");
	        actor.send(red+"Make sure you do not have an alias that conflicts with your command."+nrm);
			actor.send(" ");
			getMenu(actor,1,"smithing");
			return;
		}
	}
	else if (sval == 2)
	{
		if (strn_cmp(vArgs[0],"offensive",1))
		{
			setSval(actor,20851,"attribute",1);
			actor.detach(20854);
			actor.comm("smith "+weapon_mat+" "+weapon_type);
			return;
		}
		else if (strn_cmp(vArgs[0],"defensive",1))
		{
			setSval(actor,20851,"attribute",2);
			actor.detach(20854);
			actor.comm("smith "+weapon_mat+" "+weapon_type);
			return;
		}
		else if (strn_cmp(vArgs[0],"back",1))
		{
			setSval(actor,20851,"paging",sval-1);
			getMenu(actor,getSval(actor,20851,"paging"),"smithing");
			return;
		}
		else
		{
			actor.send("That is not a valid command!");
	        actor.send("Choose another command, or type "+cyn+"C"+nrm+" to stop smithing.");
	        actor.send(red+"Make sure you do not have an alias that conflicts with your command."+nrm);
			actor.send(" ");
			getMenu(actor,2,"smithing");
			return;
		}
	}
	else if (sval == 3)
	{
		if (strn_cmp(vArgs[0],"spear",1))
		{
			setSval(actor,20851,"attribute",7);
			actor.detach(20854);
			actor.comm("smith "+weapon_mat+" "+weapon_type);
			return;
		}
		if (strn_cmp(vArgs[0],"axe",1))
		{
			setSval(actor,20851,"attribute",8);
			actor.detach(20854);
			actor.comm("smith "+weapon_mat+" "+weapon_type);
			return;
		}
		else
		{
			actor.send("That is not a valid command!");
	        actor.send("Choose another command, or type "+cyn+"C"+nrm+" to stop smithing.");
	        actor.send(red+"Make sure you do not have an alias that conflicts with your command."+nrm);
			actor.send(" ");
			getMenu(actor,3,"smithing");
			return;
		}
	}
	else if (sval == 4)
	{
		if (strn_cmp(vArgs[0],"hit",1))
		{
			setSval(actor,20851,"attribute",10);
			actor.detach(20854);
			actor.comm("smith "+weapon_mat+" "+weapon_type);
			return;
		}
		else if (strn_cmp(vArgs[0],"damage",1))
		{
			setSval(actor,20851,"attribute",9);
			actor.detach(20854);
			actor.comm("smith "+weapon_mat+" "+weapon_type);
			return;
		}
		else
		{
			actor.send("That is not a valid command!");
	        actor.send("Choose another command, or type "+cyn+"C"+nrm+" to stop smithing.");
	        actor.send(red+"Make sure you do not have an alias that conflicts with your command."+nrm);
			actor.send(" ");
			getMenu(actor,4,"smithing");
			return;
		}
	}
	else if (sval == 5)
	{
		if (strn_cmp(vArgs[0],"dodge",1))
		{
			setSval(actor,20851,"attribute",3);
			actor.detach(20854);
			actor.comm("smith "+weapon_mat+" "+weapon_type);
			return;
		}
		else if (strn_cmp(vArgs[0],"parry",1))
		{
			setSval(actor,20851,"attribute",4);
			actor.detach(20854);
			actor.comm("smith "+weapon_mat+" "+weapon_type);
			return;
		}
		else
		{
			actor.send("That is not a valid command!");
	        actor.send("Choose another command, or type "+cyn+"C"+nrm+" to stop smithing.");
	        actor.send(red+"Make sure you do not have an alias that conflicts with your command."+nrm);
			actor.send(" ");
			getMenu(actor,5,"smithing");
			return;
		}
	}
	//Doube digit svals for ABS armor
	else if (sval == 11)
	{
		if (strn_cmp(vArgs[0],"absorption",1))
		{
			actor.detach(20854);
			actor.comm("smith "+weapon_mat+" "+weapon_type);
			return;
		}
		else if (strn_cmp(vArgs[0],"weight",1))
		{
			actor.detach(20854);
			actor.comm("smith "+weapon_mat+" "+weapon_type);
			return;
		}
		else if (strn_cmp(vArgs[0],"parry",1))
		{
			actor.detach(20854);
			actor.comm("smith "+weapon_mat+" "+weapon_type);
			return;
		}
		else if (strn_cmp(vArgs[0],"dodge",1))
		{
			actor.detach(20854);
			actor.comm("smith "+weapon_mat+" "+weapon_type);
			return;
		}
		else
		{
			actor.send("That is not a valid command!");
	        actor.send("Choose another command, or type "+cyn+"C"+nrm+" to stop smithing.");
	        actor.send(red+"Make sure you do not have an alias that conflicts with your command."+nrm);
			return;
		}
	}
}