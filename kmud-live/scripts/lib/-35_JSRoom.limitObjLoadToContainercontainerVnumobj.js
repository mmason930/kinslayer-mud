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
