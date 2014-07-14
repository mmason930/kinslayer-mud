var script13222 = function(self, actor, here, args, extra)
{
	var arg_array = getArgList(args);
if (arg_array.length < 2)
{
	actor.send("Read what?");
	_block;
	return;
}
else if (!str_cmp(arg_array[1], "map"))
{
_block;
var cyn = actor.cyan(constants.CL_OFF);	
var ylw = actor.yellow(constants.CL_OFF);
var grn = actor.green(constants.CL_OFF);
var red = actor.red(constants.CL_OFF);
var mag = actor.magenta(constants.CL_OFF);
var blu = actor.blue(constants.CL_OFF);
var bld = actor.bold(constants.CL_OFF);
var nrm = actor.normal(constants.CL_OFF);
var count = 0;
var rooms = [];
var r13203 = [4,0,mag+bld+"T"+nrm,13203,"   |","  ","___|"];
rooms.push(r13203);
var r13201 = [5,0,ylw+"*"+nrm,13201,"   |","  ","_ _|"];
rooms.push(r13201);
var r13205 = [5,1,ylw+"*"+nrm,13205,"   |"," |","_ _|"];
rooms.push(r13205);
var r13206 = [5,2,ylw+"*"+nrm,13206,"   |","  ","_ _|"];
rooms.push(r13206);
var r13211 = [5,3,ylw+"*"+nrm,13211,"   |","  ","_ _|"];
rooms.push(r13211);
var r13213 = [5,4,ylw+"*"+nrm,13213,"   |","  ","_ _|"];
rooms.push(r13213);
var r13224 = [5,5,ylw+"*"+nrm,13224,"   |","  ","_ _|"];
rooms.push(r13224);
var r13390 = [4,6,blu+bld+"C"+nrm,13390,"   |","  ","___|"];
rooms.push(r13390);
var r13230 = [5,6,ylw+"*"+nrm,13230,"   |","  ","_ _|"];
rooms.push(r13230);
var r13232 = [5,7,ylw+"*"+nrm,13232,"   |"," |","_ _|"];
rooms.push(r13232);
var r13233 = [5,8,ylw+"*"+nrm,13233,"   |","  ","_ _|"];
rooms.push(r13233);
var r13338 = [0,4,ylw+"*"+nrm,13338,"   |","  ","___|"];
rooms.push(r13338);
var r13223 = [1,4,ylw+"*"+nrm,13223,"   |","  ","_ _|"];
rooms.push(r13223);
var r13222 = [2,4,ylw+"*"+nrm,13222,"   |","  ","_ _|"];
rooms.push(r13222);
var r13221 = [3,4,ylw+"*"+nrm,13221,"   |","  ","_ _|"];
rooms.push(r13221);
var r13220 = [4,4,ylw+"*"+nrm,13220,"   |","  ","_ _|"];
rooms.push(r13220);
var r13214 = [6,4,ylw+"*"+nrm,13214,"   |","  ","___|"];
rooms.push(r13214);
var r13215 = [7,4,ylw+"*"+nrm,13215,"   |","  ","___|"];
rooms.push(r13215);
var r13217 = [8,4,ylw+"*"+nrm,13217,"   |","  ","___|"];
rooms.push(r13217);
var r13242 = [9,4,ylw+"*"+nrm,13242,"   |","  ","_ _|"];
rooms.push(r13242);
var r13245 = [10,4,ylw+"*"+nrm,13245,"   |","  ","___|"];
rooms.push(r13245);
var r13247 = [11,4,ylw+"*"+nrm,13247,"   |","  ","___|"];
rooms.push(r13247);
var r13259 = [12,4,ylw+"*"+nrm,13259,"   |","  ","___|"];
rooms.push(r13259);
var r13207 = [4,2,mag+bld+"C"+nrm,13207,"   |","  ","_ _|"];
rooms.push(r13207);
var r13208 = [4,1,grn+"*"+nrm,13208,"   |"," |","___|"];
rooms.push(r13208);
var r13229 = [6,2,mag+"S"+nrm,13229,"   |"," |","___|"];
rooms.push(r13229);
var r13225 = [6,3,mag+"I"+nrm,13225,"   |","  ","_ _|"];
rooms.push(r13225);
var r13226 = [7,3,mag+"W"+nrm,13226,"   |","  ","_ _|"];
rooms.push(r13226);
var r13227 = [8,3,mag+"W"+nrm,13227,"   |"," |","___|"];
rooms.push(r13227);
var r13228 = [7,2,mag+"W"+nrm,13228,"   |","^|","___|"];
rooms.push(r13228);
var r13387 = [4,3,grn+bld+"G"+nrm,13387,"   |","  ","___|"];
rooms.push(r13387);
var r13388 = [3,3,grn+bld+"G"+nrm,13388,"  "+cyn+bld+"*"+nrm+"|","  ","_ _|"];
rooms.push(r13388);
var r13389 = [2,3,grn+bld+"G"+nrm,13389,"   |","  ","_ _|"];
rooms.push(r13389);
var r13393 = [3,2,mag+bld+"R"+nrm,13393,"   |"," |","___|"];
rooms.push(r13393);
var r13395 = [1,3,mag+bld+"D"+nrm,13395,"   |","  ","___|"];
rooms.push(r13395);
var r13219 = [2,2,cyn+"B"+nrm,13219,"   |"," |","___|"];
rooms.push(r13219);
var r13337jump = [0,5,"|",1," | |"," |","_ _|"];
rooms.push(r13337jump);
var r13337 = [0,6,grn+"*"+nrm,13337,"   |"," |","  _|"];
rooms.push(r13337);
var r13336 = [0,7,grn+"*"+nrm,13336,"   |","  ","_ _|"];
rooms.push(r13336);
var r13334 = [0,8,grn+"*"+nrm,13334,"   |","  ","_ _|"];
rooms.push(r13334);
var r13333 = [1,8,grn+"*"+nrm,13333,"   |","  ","_ _|"];
rooms.push(r13333);
var r13335 = [1,7,grn+"*"+nrm,13335,"   |"," |","___|"];
rooms.push(r13335);
var r13332 = [2,8,grn+"*"+nrm,13332,"   |","  ","___|"];
rooms.push(r13332);
var r13333jump = [3,8," ",2,"---|","  ","---|"];
rooms.push(r13333jump);
var r13233jump = [4,8," ",2,"---|","  ","---|"];
rooms.push(r13233jump);
var r13301jump = [6,8," ",2,"---|","  ","---|"];
rooms.push(r13301jump);
var r13301 = [7,8,grn+"*"+nrm,13301,"   |"," |","___|"];
rooms.push(r13301);
var r13256 = [9,8,grn+"*"+nrm,13256,"   |","  ","_ _|"];
rooms.push(r13256);
var r13258 = [8,8,grn+"*"+nrm,13258,"   |","  ","___|"];
rooms.push(r13258);
var r13257 = [10,8,grn+"*"+nrm,13257,"   |","  ","___|"];
rooms.push(r13257);
var r13288 = [11,8,grn+"*"+nrm,13288,"  "+cyn+bld+"*"+nrm+"|"," |","___|"];
rooms.push(r13288);
var r13255 = [9,7,grn+"*"+nrm,13255,"   |"," |","_ _|"];
rooms.push(r13255);
var r13254 = [9,6,grn+"*"+nrm,13254,"   |","  ","___|"];
rooms.push(r13254);
var r13253 = [10,6,grn+"*"+nrm,13253,"   |","  ","___|"];
rooms.push(r13253);
var r13249 = [11,6,grn+"*"+nrm,13249,"   |","  ","_ _|"];
rooms.push(r13249);
var r13250 = [12,6,grn+"*"+nrm,13250,"   |"," |","___|"];
rooms.push(r13250);
var r13248 = [11,5,grn+"*"+nrm,13248,"   |"," |","_ _|"];
rooms.push(r13248);
var r13252 = [11,7,grn+"*"+nrm,13252,"   |","  ","_ _|"];
rooms.push(r13252);
var r13251 = [12,7,grn+"*"+nrm,13251,"   |"," |","_ _|"];
rooms.push(r13251);
var r13243 = [9,3,blu+bld+"A"+nrm,13243,"   |"," |","___|"];
rooms.push(r13243);
var r13204 = [6,0,mag+bld+"W"+nrm,13204,"   |"," |","___|"];
rooms.push(r13204);
var r13234 = [11,0,mag+"W"+nrm,13234,"   |"," |","___|"];
rooms.push(r13234);
var r13235 = [11,1,mag+"W"+nrm,13235,"   |","  ","_ _|"];
rooms.push(r13235);
var r13238 = [11,2,mag+"W"+nrm,13238,"   |","  ","_ _|"];
rooms.push(r13238);
var r13239 = [11,3,mag+"W"+nrm,13239,"   |"," |","_ _|"];
rooms.push(r13239);
var r13236 = [10,1,mag+"W"+nrm,13236,"   |","  ","___|"];
rooms.push(r13236);
var r13237 = [12,1,mag+"W"+nrm,13237,"   |"," |","___|"];
rooms.push(r13237);
var r13240 = [10,2,mag+"W"+nrm,13240,"   |","  ","___|"];
rooms.push(r13240);
var r13241 = [12,2,mag+"W"+nrm,13241,"   |"," |","___|"];
rooms.push(r13241);
var r13244 = [9,5,blu+bld+"W"+nrm,13244,"   |","  ","_ _|"];
rooms.push(r13244);
var r13246 = [10,5,blu+bld+"W"+nrm,13246,"   |"," |","_ _|"];
rooms.push(r13246);
var r13218 = [8,5,blu+bld+"G"+nrm,13218,"   |"," |","_ _|"];
rooms.push(r13218);
var r13216 = [7,5,blu+bld+"T"+nrm,13216,"   |"," |","_ _|"];
rooms.push(r13216);
var r13396 = [7,6,blu+bld+"C"+nrm,13396,"   |"," |","_ _|"];
rooms.push(r13396);
var r13392 = [6,5,cyn+"B"+nrm,13392,"   |"," |","___|"];
rooms.push(r13392);
var r13231 = [6,6,cyn+"B"+nrm,13231,"   |"," |","___|"];
rooms.push(r13231);
var r13391 = [4,7,cyn+"B"+nrm,13391,"   |","  ","___|"];
rooms.push(r13391);
var r13210 = [3,5,cyn+"B"+nrm,13210,"   |"," |","_ _|"];
rooms.push(r13210);
var r13209 = [2,5,cyn+"B"+nrm,13209,"   |"," |","_ _|"];
rooms.push(r13209);
var rtitle1 = [0,1,bld+" "+nrm,3,"    "," "+bld+" "+nrm," "+bld+"E M"+nrm];
rooms.push(rtitle1);
var rtitle2 = [1,1,bld+" "+nrm,3,"    "," "+bld+" "+nrm," "+bld+"O N"+nrm];
rooms.push(rtitle2);
var rtitle3 = [2,1,bld+" "+nrm,3,"    ",bld+"  "+nrm," "+bld+"D '"+nrm];
rooms.push(rtitle3);
var rtitle4 = [0,0,grn+bld+" "+nrm,3," "+bld+"  F"+nrm,"  ","____"];
rooms.push(rtitle4);
var rtitle5 = [1,0,grn+bld+" "+nrm,3," "+bld+"I E"+nrm,"  ","____"];
rooms.push(rtitle5);
var rtitle6 = [2,0,grn+bld+" "+nrm,3," "+bld+"L D"+nrm,"  ","____"];
rooms.push(rtitle6);
var rtitle7 = [3,0,grn+bld+" "+nrm,3,"   |"," |","___|"];
rooms.push(rtitle7);
var rtitle8 = [3,1,bld+" "+nrm,3,"   |"," |"," "+bld+"S"+nrm+" |"];
rooms.push(rtitle8);
actor.send("You start studying " + self.name + ".");
echoaround(actor, actor.name + " starts studying " + self.name + ".");
for each (var r in rooms)
{
	//here.echo(actor.room.vnum);
	if (actor.room.vnum == r[3])
	{
		var actor_room = [r[0],r[1]];
		//here.echo(actor_room);
	}
}
var buffer = "";
actor.send(" ");
buffer += " _____________________ _______ _______ ___ ___ _____\n";
for (var y = 8; y >= 0; --y)
{
	//Top line.
	buffer += "|";
	for (var x = 0; x <= 12; ++x)
	{
		for each(var r in rooms)
		{
			if (r[0] == x && r[1] == y)
			{
				
				if (r[3] == 1)
					buffer += " | |"
				else if (r[3] == 2)
					buffer += "___|";
				else
					buffer += r[4];
				count += 1;
			}
		}
		if (count > 0)
			count = 0;
		else
			buffer += "   |";
	}
	if (y == 8)
		buffer += "   "+red+bld+"X"+nrm+"  - You are here\n";
	else if (y == 7)
		buffer += "   "+mag+"I"+nrm+"  - Innkeeper\n";
	else if (y == 6)
		buffer += "\n";
	else if (y == 5)
		buffer += "   "+mag+bld+"W"+nrm+"  - Warrior Trainer\n";
	else if (y == 4)
		buffer += "   "+mag+bld+"T"+nrm+"  - Thief Trainer\n";
	else if (y == 3)
		buffer += "   "+blu+bld+"C"+nrm+"  - Crafted Goods\n";
	else if (y == 2)
		buffer += "   "+blu+bld+"A"+nrm+"  - Armorer\n";
	else if (y == 1)
		buffer += "        "+ylw+"Other Info"+nrm+"\n";
	else if (y == 0)
		buffer += "   "+cyn+"B"+nrm+"  - Building\n";
	//Middle line, marked with X's
	buffer += "|";
	for (var x = 0; x <= 12; ++x)
	{
		// var coor = [];
		// coor.push(x);
		// coor.push(y);
		for each(var r in rooms)
		{
			if (actor.room.vnum == r[3])
			{
				if (r[0] == x && r[1] == y)	
				{
					if (x == 11 && y == 0)
						buffer += "^"+bld+red+"X"+nrm+r[5];
					else
						buffer += " "+bld+red+"X"+nrm+r[5];
					count += 1;
				}
			}
			else if (r[0] == x && r[1] == y)
			{
				if (r[3] == 1)
					buffer += " | |"
				else if (r[3] == 2)
					buffer += "___ ";
				else
				{
					if (x == 11 && y == 0)
						buffer += "^"+r[2]+r[5];
					else
						buffer += " "+r[2]+r[5];
				}
				count += 1;
			}
		}
		if (count > 0)
			count = 0;
		else
			buffer += "   |";
	}
	if (y == 8)
		buffer += "\n";
	else if (y == 7)
		buffer += "   "+mag+"W"+nrm+"  - The Winespring Inn\n";
	else if (y == 6)
		buffer += "        "+mag+bld+"Trainers"+nrm+"\n";
	else if (y == 5)
		buffer += "   "+mag+bld+"R"+nrm+"  - Ranger Trainer\n";
	else if (y == 4)
		buffer += "\n";
	else if (y == 3)
		buffer += "   "+blu+bld+"G"+nrm+"  - Grocer\n";
	else if (y == 2)
		buffer += "   "+blu+bld+"W"+nrm+"  - Weaponsmith\n";
	else if (y == 1)
		buffer += "   "+ylw+"*"+nrm+"  - Road\n";
	else if (y == 0)
		buffer += "   "+grn+bld+"G"+nrm+"  - The Green\n";
	//Bottom line.
	buffer += "|";
	for (var x = 0; x <= 12; ++x)
	{
		for each(var r in rooms)
		{
			if (r[0] == x && r[1] == y)
			{
				if (r[3] == 1)
					buffer += " |_|"
				else if (r[3] == 2)
					buffer += "___|";
				else
					buffer += r[6];
				count += 1;
			}
		}
		if (count > 0)
			count = 0;
		else
			buffer += "___|";
	}
	if (y == 8)
		buffer += "        "+mag+"Inn and Stables"+nrm+"\n";
	else if (y == 7)
		buffer += "   "+mag+"S"+nrm+"  - Stables\n";
	else if (y == 6)
		buffer += "   "+mag+bld+"D"+nrm+"  - Trades Trainer\n";
	else if (y == 5)
		buffer += "   "+mag+bld+"C"+nrm+"  - Channeler Trainer\n";
	else if (y == 4)
		buffer += "        "+blu+bld+"Shops"+nrm+"\n";
	else if (y == 3)
		buffer += "   "+blu+bld+"T"+nrm+"  - Tailor\n";
	else if (y == 2)
		buffer += "\n";
	else if (y == 1)
		buffer += "   "+grn+"*"+nrm+"  - Wilderness\n";
	else if (y == 0)
		buffer += "   "+cyn+bld+"*"+nrm+"  - Water Source\n";
}
actor.send(buffer);
actor.send(" ");
actor.send(cyn+bld+"Make sure your text is wrapped at column 80+ to view the map correctly."+nrm);
}
else
{
	_noblock;
	return;
}




};

