var script23200 = function(self, actor, here, args, extra) {
	_block;
	var ship = getMyShip(self);
	var sVnum = ship.vnum;
	var otherRoom = ship.getOtherShip();
	var x1 = getSval(self, 23201, "x");
	var y1 = getSval(self, 23201, "y");
	var x2 = getSval(otherRoom, 23201, "x");
	var y2 = getSval(otherRoom, 23201, "y");
	//Given two coordinates, print the map to actor...
	//Each box has a right side and a bottom. We need to print the top first.
	var buffer = "";
	buffer += " _______________________________________\n";
	//Top to bottom
	for(var y = 9;y >= 0;--y) {
	    //Print the left side before we do anything.
	    buffer += "|";
	    //Left to right
	    for(var x = 0;x <= 9;++x) {
	        buffer += "_";
	        if( (y1 == y && x1 == x) ) {
	        //My Ship
				buffer +=  actor.bold(constants.CL_SPARSE) + actor.green(constants.CL_SPARSE) + "X" + actor.normal(constants.CL_SPARSE);
	        }
			else if( (y2 == y && x2 == x) ) {
			//Other Ship
				buffer +=  actor.bold(constants.CL_SPARSE) + actor.magenta(constants.CL_SPARSE) + "X" + actor.normal(constants.CL_SPARSE);
			}
	        else {
	        //Empty
	            buffer += "_";
	        }
	        buffer += "_|";
	    }
	    buffer += "\n";
	}
	actor.send(buffer);
}