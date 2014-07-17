JSRoom.prototype.closeDoor = function(dir,bOtherSide)
{
    this.setDoorFlags( dir, this.doorFlags(dir) | (1<<1) );
    if( bOtherSide ) {
        var otherRoom = this.direction( dir );
        if( otherRoom ) {
            otherRoom.closeDoor( revDir(dir), false );
        }
    }
}

JSRoom.prototype.openDoor = function(dir,bBothSides)
{
    if( this.doorExists(dir) ) {
        var flags = this.doorFlags(dir);
        flags &= ~(1<<1);
        this.setDoorFlags( dir, flags );
    }
    if( bBothSides ) {
        var otherRoom = this.direction( dir );
        if( otherRoom ) {
            otherRoom.openDoor( revDir(dir,false) );//Open the connected room's door, but only one-way.
        }
    }
}

JSRoom.prototype.unlockDoor = function(dir,bOtherSide)
{
    if( this.doorExists(dir) && this.doorIsClosed(dir) ) {
        var flags = this.doorFlags(dir);
        flags &= ~(1<<2);
        this.setDoorFlags( dir, flags );

        if( bOtherSide ) {
            var otherRoom = this.direction(dir);
            var otherDir = revDir(dir);
            if( otherRoom && otherRoom.doorExists(otherDir) && otherRoom.doorIsClosed(otherDir) ) {
                otherRoom.unlockDoor(otherDir,false);
            }
        }
    }
}

JSRoom.prototype.lockDoor = function(dir, bOtherSide)
{
    if( this.doorExists(dir) && this.doorIsClosed(dir) ) {
        this.setDoorFlags( dir, this.doorFlags(dir) | (1<<2) );
    }
    if( bOtherSide ) {
        var otherRoom = this.direction(dir);
        var otherDir = revDir(dir);
        if( otherRoom && otherRoom.doorExists(otherDir) && otherRoom.doorIsClosed(otherDir) ) {
            otherRoom.lockDoor(otherDir,false);
        }
    }
}

//Alder December 2009
/**
 This function is for a JSRoom and acts similar to the stacking option in ZEDIT.
 You simply supply the vnum of the container (object or mob) you are loading objects to, the vnum
 of the object you are loading, and the number of the given object that the container
 can have. As long as the number of the specified object is less than the max, then
 it will load in the container whenever your script is triggered.
 **/

JSRoom.prototype.limitObjLoadToContainer = function( containerVnum, objVnumToLoad, objMax ) {
    var container = 0;//This variable will store JSObject of container
    var items = this.items;//Array of items in room
    var people = this.people;//Array of people in room
    /**Loop through room to find container as item**/
    for ( var i = 0; i < items.length; i++ ) {
        if ( items[i].vnum == containerVnum ) {
            container = items[i];//Passed container is item in room
            var containerItems = container.contents;
            break;
        }
    }
    /**If the container is not an item in the room, check for character**/
    if ( !container ) {
        for ( var i = 0; i < people.length; i++ ) {
            if ( people[i].vnum == containerVnum ) {
                container = people[i];//Passed container is character in room
                var containerItems =  container.inventory;
                break;
            }
        }
    }
    if ( !container )//Specified container vnum is not that of an item or character in room
        return;
    var objCounter = 0;//This variable is a counter for the number of the given object in container
    /**Loop through container contents and see if specified object limit has already been reached**/
    for ( var i = 0; i < containerItems.length; i++ ) {
        if ( containerItems[i].vnum == objVnumToLoad )
            objCounter++;//Each time the specified object is found in container, increment the counter
    }
    if ( objCounter >= objMax )//Don't load another object if limit is reached
        return;
    container.loadObj(objVnumToLoad);//Load another object to container
}

//Alder December 2009
/**
 Returns -1 if no doors in room have names that match supplied string.
 Returns int dir if a door in room has name that matches supplied string.
 **/
JSRoom.prototype.doorName_cmp = function( strDoorName ) {
    /**0=north, 1=east...**/
    for ( var i = 0; i < 6; i++ )
    {
        if ( this.doorExists(i) )
        {
            if ( !str_cmp( this.doorName(i), strDoorName ) )//Check next dir if supplied name is not door name
                return i;//Return direction int if supplied name is door name
        }
    }
    return -1;//The supplied door name did not match any doors in the room
}

// Purges all of the objects in a room. If skipChests is true, skips all objects flagged with ITEM_CHEST
JSRoom.prototype.purgeItems = function( skipChests) {
    for each(var obj in this.items) {
        if( !skipChests || !obj.extraFlags(constants.ITEM_CHEST) )
            obj.extract();
    }
}
JSRoom.prototype.purgeMobs = function() {
    for each(var mob in this.people) {
        if (mob.vnum > 0)
            mob.extract();
    }
}

// Returns an array of all the connected players within a zone with the specified vnum
function getPlayersInZone(zone) {
    var players = new Array();
    for each(var person in getConnectedPlayers()) {
        if( person.room.zoneVnum == zone ) {
            players.push(person);
        }
    }
    return players;
}

//Alder
//June 2011
/** Returns a random direction constant that leads to one of the room's exits **/
JSRoom.prototype.getRandomExitDirection = function () {
    var exits = this.neighbors;
    for ( var i = 0; i < exits.length; i++ ) {
        if ( exits[i] == undefined ) {
            exits.splice(i,1);
            i--;
        }
    }
    return this.firstStep(exits[random(0,exits.length-1)]);
}

/**************************************************************************
 *                                                                        *
 * JSRoom.echoToNeighbors                                                 *
 * Arguments: - sMessage: The message to be sent to people in the room.   *
 *               This can be either a function, in which chase it will    *
 *               take in a single argument, JSCharacter, which will return*
 *               the message to be sent to the player, or it can be a     *
 *               string.                                                  *
 *            - iDepth: The radius to which the message reverberates.     *
 *            - fPredicate: A function that takes in a JSCharacter        *
 *               argument. Returns true if the msg is to be sent, and     *
 *               false if not.        ( OPTIONAL )                        *
 * Purpose: Sends a message to neighboring rooms up to  specified depth   *
 *                                                                        *
 * ~~~ By: Galnor 03/20/2010                                              *
 *                                                                        *
 **************************************************************************/
JSRoom.prototype.echoToNeighbors = function(sMessage, iDepth, fPredicate)
{
    var fMessage = undefined;
    if( getObjectClass(sMessage) == "Function" )
        fMessage = sMessage;
    var roomsSentTo = [ this ];
    var roomsRemaining = [];

    var neighbors = this.neighbors;
    for(var i = 0;i < neighbors.length;++i) {
        if( neighbors[ i ] == undefined ) continue;
        var obj = new Object();
        obj.depth = 1;
        obj.room = neighbors[ i ];
        roomsRemaining.push( obj );
    }

    while( roomsRemaining.length > 0 ) {
        var rObj = roomsRemaining[ 0 ];
        var iDepthInThisRoom = rObj.depth;
        var thisRoom = rObj.room;
        roomsSentTo.push( thisRoom );
        roomsRemaining.splice( 0, 1 );

        //mudLog(constants.BRF, 100, "Trying room #" + thisRoom.vnum);

        var people = thisRoom.people;
        for(var i = 0; i < people.length;++i) {
            if( fPredicate == undefined || fPredicate( people[i] ) )
            {
                if( fMessage != undefined )
                    sMessage = fMessage( people[i] );
                people[ i ].send( sMessage );
            }
        }
        /*** Queue up the neighboring rooms ***/
        if( iDepthInThisRoom < iDepth )
        {
            var neighbors = thisRoom.neighbors;
            for(var i = 0;i < neighbors.length;++i) {
                if( neighbors[ i ] == undefined ) continue;
                //Look to see if we've already touched this room.
                var touched = false;
                for(var r = 0;r < roomsSentTo.length;++r) {
                    if( roomsSentTo[ r ] == neighbors[ i ] )
                    {
                        touched = true;
                        break;
                    }
                }
                if( touched == false ) {
                    var obj = new Object();
                    obj.depth = iDepthInThisRoom + 1;
                    obj.room = neighbors[ i ];
                    roomsRemaining.push( obj );
                }
            }
        }
    }
}

JSRoom.prototype.getMobs = function( mobVnum )
{
    var mobVnumArray = typeof(mobVnum) == "number" ? [mobVnum] : mobVnum;
    var mobVnumMap = {};
    var mobResultArray = [];

    //Convert to map with mob virtual number as the index to increase lookup speed below.
    for each(var tempMobVnum in mobVnumArray)
    {
        mobVnumMap[ tempMobVnum ] = true;
    }

    for each(var character in this.people)
    {
        if(mobVnumMap[ character.vnum ] === true)
            mobResultArray.push(character);
    }

    return mobResultArray;
}

JSRoom.prototype.getObjects = function(objectVnum)
{
    var objectVnumArray = typeof(objectVnum) == "number" ? [objectVnum] : objectVnum;
    var objectVnumMap = {};
    var objectResultArray = [];

    //Convert to map with mob virtual number as the index to increase lookup speed below.
    for each(var tempObjectVnum in objectVnumArray)
    {
        objectVnumMap[ tempObjectVnum ] = true;
    }

    for each(var object in this.items)
    {
        if(objectVnumMap[ object.vnum ] === true)
            objectResultArray.push(object);
    }

    return objectResultArray;
}
