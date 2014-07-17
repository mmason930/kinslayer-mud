var script23265 = function(self, actor, here, args, extra) {
		var vArgs = getArgList(args);
		if ( vArgs.length != 2 ) //This script requires two arguments; nothing more, nothing less
			return;
		var cannon = getObjInListVis(actor, vArgs[1], here.items);
		if ( !cannon || cannon.vnum != 23263 ) //Check to see if second argument is an alias of cannon
			return;
		_block;
		if ( getSval(self, 23263, "loaded") != 1 ) //Check to see if cannon is loaded
		{
			actor.send("Perhaps you should load it first.");
			return;
		}
		actor.send("You attempt to light the fuse on " + self.name + ".");
		here.echoaround(actor, actor.name + " moves to light the fuse on " + self.name + ".");
		actor.startTimer(1);
		var success = runTimer(actor);
		if (success)
		{
			here.echo("\nThe fuse on " + self.name + " ignites and burns rapidly...");
			setSval(self, 23263, "loaded", 0);
		}
		else
		{
				here.echo("\nThe fuse on " + self.name + " fails to light.");
				return;
		}
		wait 1;
		//This bit looks at the name of the cannon's room to determine what to do with the coordinates later;IE: if "north" add to Y
		if( here.name == "The Stern's Cannon" )
			var ball_dir = "south";
		else if( here.name == "At the Cannon on the Eastern Rail")
			var ball_dir = "east";
		else if( here.name == "The Cannon at the Bow")
			var ball_dir = "north";
		else if( here.name == "At the Cannon on the Western Rail")
			var ball_dir = "west";
		var this_ship = getMyShip(here);
		var other_ship = this_ship.getOtherShip();
		var this_X = this_ship.getShipX();
		var this_Y = this_ship.getShipY();
		var other_X = other_ship.getShipX();
		var other_Y = other_ship.getShipY();
		//Establish the number of rooms that the cannonball COULD land in; depending on the direction, max distance will be distance from coordinate to nine or zero
		if ( !str_cmp(ball_dir, "south") )
			var max_shoot_dist = ( this_Y ); //(Y to bottom)
		else if ( !str_cmp(ball_dir, "north") )
			var max_shoot_dist = ( 9 - this_Y ); //(Top to Y)
		else if ( !str_cmp(ball_dir, "west") )
			var max_shoot_dist = ( this_X ); //(X to left edge)
		else if ( !str_cmp(ball_dir, "east") )
			var max_shoot_dist = ( 9 - this_X ); //(Right edge to X)
		//Portion below determines the number to add/subtract from the coordinate and thus the room the cannonball will land in
		var max = (max_shoot_dist^2);
		var adj_percent = ( max/2 );//The adjacent room will have a constant landing rate based on the maximum shooting distance
		var land_rate = random(1, max );
		for( var i = 1; i <= max_shoot_dist; i++ )
		{
			if ( max_shoot_dist == 1 )
			{
				var shoot_dist = 1;
				break;
			}
			var op = ( max - ( adj_percent/(i) ) );
			if ( land_rate < op )
				var shoot_dist = i;
		}
		if ( shoot_dist == undefined )
			var shoot_dist = random(1,max_shoot_dist); //If the shooting distance has not been determined, pick a random distance allowed by max distance
		//Here the coordinates of where the cannonball will land are stored to variables
		if ( !str_cmp(ball_dir, "south") ) 
		{
			var landing_room_X = this_X;
			var landing_room_Y = (this_Y - shoot_dist);
		}
		else if ( !str_cmp(ball_dir, "north") ) {
			var landing_room_X = this_X;
			var landing_room_Y = (this_Y + shoot_dist);
		}
		else if ( !str_cmp(ball_dir, "west") ) {
			var landing_room_X = (this_X - shoot_dist);
			var landing_room_Y = this_Y;
		}
		else if ( !str_cmp(ball_dir, "east") ) {
			var landing_room_X = (this_X + shoot_dist);
			var landing_room_Y = this_Y;
		}
		this_ship.myRooms;
		var rooms = this_ship.myRooms;
		//Echo the sound of a cannon blast in every room EXCLUDING this one. This room has a special message sent to each person in it.
		for(var i = 0;i < rooms.length;i++)
		{
			var room = getRoom(rooms[i]);
			if ( room != here )
				room.echo("You hear a loud blast as a cannon fires somewhere on the ship.");
			else 
			{
				var people = room.people;
				for( var p = 0; p < people.length; p++ )
					people[p].send( people[p].yellow(constants.CL_SPARSE) + "BOOM!!!" + people[p].normal(constants.CL_SPARSE) + "\nA cannonball is sent flying off to the " + ball_dir + ".");
			}
		}
		if ( landing_room_X != other_X || landing_room_Y != other_Y )
		{
			wait 3;
			for ( p = 0; p < here.people.length; p++ )
				here.people[p].send( here.people[p].red(constants.CL_SPARSE) + "MISS..." + here.people[p].normal(constants.CL_SPARSE) );
			var water_impact = getRoom( (23300 + (landing_room_X * 10) + landing_room_Y) );
			if ( here.people ) {
			if ( random(1,5) < 2 )
			{
				var water_death = getRandomPersonInRoom(water_impact);
				if( water_death ) {
					water_death.send("You hear a faint whistle from above before a speeding cannonball smashes into your face!");
					water_impact.echoaround(water_death, "A faint whistle can be heard just before " + water_death.name + "'s head is destroyed by an errant cannonball!");
					water_death.hps -= 500;
				}
			}
			water_impact.echo("A speeding cannonball smacks into the water nearby, creating an enormous splash!"); }
		}
		else //Cannon hit the ship
		{
			for(var i = 0;i < rooms.length;i++)
			{
				var room = getRoom(rooms[i]);
				var people = room.people;
				for( var p = 0; p < people.length; p++ )
					people[p].send( people[p].green(constants.CL_SPARSE) + "The enemy ship has been HIT!"  + people[p].normal(constants.CL_SPARSE) );
			}
			other_ship.sendToOutdoors("A high-pitched whistling sound can be heard approaching over the water...");
			wait 2;
			//Below is where the cannonball will land on the ship. The impact has a one-room radius from the initial room.
			var other_rooms = other_ship.myRooms;
			var blast_site = getRoom( other_rooms[ random(0, other_rooms.length - 1) ] ); //Get a random room on the ship
			var secondary_vict = blast_site.people;
			for ( var v = 0; v < secondary_vict.length; v++ )
			{
				if ( v == 0 ) { other_ship.sendToOutdoors("An enemy cannonball explodes on the ship!"); }
				secondary_vict[v].send(secondary_vict[v].yellow(constants.CL_SPARSE) + "BOOOM!!!" + secondary_vict[v].normal(constants.CL_SPARSE));
				secondary_vict[v].hps -= 400;
				if ( secondary_vict[v].room == blast_site )//If victim is still alive, teleport him to a random room on ship
				{
					secondary_vict[v].send("Smoke and splinters of wood cloud your vision as the force of the blast sends you flying!");
					var poss_rooms = getRoom( other_rooms[ random(0, other_rooms.length - 1) ] );
					if ( poss_rooms.sector == constants.SECT_CITY )//If a blast room is on the deck, potential to be thrown overboard
					{
						if ( random(1,5) < 3 )
						{
							var surr_water = other_ship.getWaterRoom().neighbors;
							secondary_vict[v].send("The force of the blast sends you flying overboard!");
							secondary_vict[v].moveToRoom(getRoom(surr_water[random(0,surr_water.length - 1)]));
						}
					}
					else
						secondary_vict[v].moveToRoom( poss_rooms );
					secondary_vict[v].comm("look");
				}
			}	
			var radius_sites = blast_site.neighbors;
			for ( var n = 0; n < radius_sites.length; n++ )
			{
				if ( !radius_sites[n] )
					continue;
				var site = radius_sites[n];
				var secondary_vict = site.people;
				for ( var v = 0; v < secondary_vict.length; v++ )
				{
					secondary_vict[v].send( secondary_vict[v].yellow(constants.CL_SPARSE) + "BOOOM!!!" + secondary_vict[v].normal(constants.CL_SPARSE) );
					secondary_vict[v].hps -= 300;
					if ( secondary_vict[v].room == radius_site[n] )//If victim is still alive, teleport him to a random room on ship
					{
						secondary_vict[v].send("Smoke and splinters of wood cloud your vision as the force of the blast sends you flying!");
						var poss_rooms = getRoom( other_rooms[ random(0, other_rooms.length - 1) ] );
						if ( poss_rooms.sector == constants.SECT_CITY )//If a blast room is on the deck, potential to be thrown overboard
						{
							if ( random(1,5) < 3 )
							{
								var surr_water = other_ship.getWaterRoom().neighbors;
								for ( var w = 0; w < surr_water; w++ )
								{
									if ( !surr_water[w] )
										continue;
									secondary_vict[v].send("The force of the blast sends you flying overboard!");
									secondary_vict[v].moveToRoom(getRoom(surr_water[random(0,surr_water.length - 1)]));
								}
							}
						}
						else
							secondary_vict[v].moveToRoom( poss_rooms );
						secondary_vict[v].comm("look");
					}
				}
			}
		
		}
	
}