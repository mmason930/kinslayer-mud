var script23210 = function(self, actor, here, args, extra)
{
	/**** Setup all ship utilities. We are basically turning the room into a pseudo 'ship' class. ****/
if( !self.init ) {
    self.getOtherShip = function() {
        if( self.vnum == 23239 )
            return getRoom( 23210 );
        return getRoom(23239);
    }
    //Send output to the entire ship.
    self.sendToShip = function( str, exclusion ) {
        for(var i = 0;i < self.myRooms.length;++i) {
            var excluded = false;
            if( exclusion )
            {
                for(var p = 0;p < exclusion.length;++p) {
                    if( exclusion[p] == self.myRooms[i] ) {
                        excluded = true;
                        break;
		    }
                }
            }
            if( !excluded )
                getRoom(self.myRooms[i]).echo( str );
        }
    }
    self.sendToIndoors = function( str ) {
        self.sendToShip( str, self.getOutdoorRooms() );
    }
    self.sendToOutdoors = function( str ) {
        self.sendToShip( str, self.getIndoorRooms() );
    }
    self.getIndoorRooms = function() {
        var rArr = [];
        for(var i = 0;i < self.myRooms.length;++i) {
            if( getRoom(self.myRooms[i]).sector == constants.SECT_INSIDE )
                rArr.push(self.myRooms[i]);
        }
        return rArr;
    }
    self.getOutdoorRooms = function() {
        var rArr = [];
        for(var i = 0;i < self.myRooms.length;++i) {
            if( getRoom(self.myRooms[i]).sector != constants.SECT_INSIDE )
                rArr.push(self.myRooms[i]);
        }
        return rArr;
    }
    self.moveShipForward = function()
    {
        if( self.shipDirection == undefined )
            return;
        var x = self.getShipX();
        var y = self.getShipY();
        var xO = self.getOtherShip().getShipX();
        var yO = self.getOtherShip().getShipY();
        var dir = self.shipDirection;
        
        if( dir == 0 ) ++y;
        if( dir == 1 ) ++x;
        if( dir == 2 ) --y;
        if( dir == 3 ) --x;
    
        if( x >= 10 || y >= 10 || x < 0 || y < 0 || (x == xO && y == yO) ) {
            self.collideWith( x, y );
            return;
        }
        self.setShipCoordinates(x,y);
        self.sendToOutdoors("The ship drifts slowly to the " + dirToText(dir) + ".", []);
        self.sendToIndoors("The walls creak softly as the ship wobbles forward.");
    }
    self.getWaterRoom = function() {
        return 23300 + (self.getShipX() * 10) + self.getShipY();
    }
    self.collideWith = function( tileX, tileY )
    {
        var os = self.getOtherShip();
        if( os.x == tileX && os.y == tileY )
            os.collideWith(self.getShipX(), self.getShipY());
        self.sendToShip("Everything seems to lurch forward as the ship SLAMS into something!", []);
        
        var rooms = self.myRooms;
        //Go through EVERY room and throw people forward.
        var peopleAlreadyMoved = [];
        for(var i = 0;i < rooms.length;++i) {
            var room = getRoom(rooms[i]);
            var people = room.people;
            for( var p = 0;p < people.length;++p )
            {
		var alreadyMoved = false;
                for(var j = 0;j < peopleAlreadyMoved.length;++j)
                {
	            if(peopleAlreadyMoved[j] == people[p])
                    {
                        alreadyMoved = true;
                        break;
                    }
                }
                if( alreadyMoved )
		    continue;
                peopleAlreadyMoved.push(people[p]);
                //Alright, this is a little tricky. We'll want to throw them the same direction the ship is moving.
                var neighbor = room.direction(self.shipDirection);
                if( neighbor == null ) {
                    //No neighboring room. We'll need to see if they're up on deck. If so, they're getting tossed overboard.
                    if( room.sector == constants.SECT_INSIDE ) {
                        people[p].send("You are thrown against something hard!");
                        people[p].damage(random(10,50));
                    }
                    else
                    {//Into the water with ye!
                        people[p].send("The force of the collision throws you overboard!");
                        people[p].moveToRoom(getRoom(self.getWaterRoom()));
                        people[p].comm("look");
                        
                        room.echo(people[p].name + " stumbles over the edge of the ship, falling into the water!");
                        people[p].room.echoaround(people[p], "SPLASH! Something lands nearby in the water.");
                    }
                } else {
                    people[p].send("You fall forward from the force of the collision!");
                    people[p].moveToRoom(neighbor);
                    people[p].comm("look");
                    
                    room.echo(people[p].name + " goes flying forward.");
                    people[p].room.echoaround(people[p], people[p].name + " stumbles into the room, barely keeping balance.");
                }
            }
        }
        //Turn the ship backwards...
        self.shipDirection = revDir(self.shipDirection);
    }
    self.setShipCoordinates = function( x, y ) {
        setSval(self, 23201, "x", x);
        setSval(self, 23201, "y", y);
    }
    self.getShipX = function() {
        return getSval(self, 23201, "x");
    }
    self.getShipY = function() {
        return getSval(self, 23201, "y");
    }
    self.roomInShip = function( room ) {
        for(var i = 0;i < self.myRooms.length;++i) {
            if( self.myRooms[i] == room.vnum )
                return true;
        }
        return false;
    }
    
    if( self.vnum == 23210 ) {
        self.myRooms = [23201,23202,23203,23204,23205,23206,23207,23208,23209,23210,23211,23213,23214,23215,
                        23216,23217,23218,23219,23220,23224,23225,23226,23227,23228,23229,23230,23231,23232,
                        23233,23234];
        self.setShipCoordinates(0,0);
    }
    else {
        self.myRooms = [23250,23236,23257,23255,23254,23253,23256,23252,23251,23249,23246,23247,23248,23245,
                        23244,23240,23239,23238,23241,23242,23243,23237,23258,23259,23260,23261,23262,23263,
                        23264,23265];
        self.setShipCoordinates(9,9);
    }
    self.anchored = true;
    self.init = true;
}

};

