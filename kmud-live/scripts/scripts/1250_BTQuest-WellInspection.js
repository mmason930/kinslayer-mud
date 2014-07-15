var script1250 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	var nrRows = 3;
	var nrCols = 3;
	if( vArgs.length > 1 && isName(vArgs[1], self.namelist) )
	{//Inspection succeeds.
	    _block;
		var sBuf = "You look at the bricks on the well:\n";
		var brickCount = 0;
	    for(var i = 0;i < nrRows;++i)
		{//Top to bottom
		    sBuf +=  " ______ ______ ______\n";
			for(var p = 0;p < 3;++p)
			{//3 rows
		        for(var j = 0;j < nrCols;++j)
			    {//Left to right
				    if( p == 2 )
					    sBuf += "|______";
					else if( p == 1 )
					{
				        ++brickCount;
				        var tile = getSval(self, 1250, "brick" + brickCount);
						if     ( tile == "blue" )
						    tile = actor.cyan(constants.CL_SPARSE) + "<FULL>" + actor.normal(constants.CL_SPARSE);
						else if( tile == "green" )
						    tile = actor.green(constants.CL_SPARSE) + "<FULL>" + actor.normal(constants.CL_SPARSE);
						else if( tile == "red" )
						    tile = actor.magenta(constants.CL_SPARSE) + "<FULL>" + actor.normal(constants.CL_SPARSE);
						else
						    tile = "<NONE>";
						sBuf += ("|" + tile);
					}
					else
	    			    sBuf += "|      ";
			    }
			    sBuf += "|\n";
			}
		}
	//	sBuf +=  " ______ ______ ______\n";
		actor.send(sBuf);
	}
	else
	    _noblock;
}