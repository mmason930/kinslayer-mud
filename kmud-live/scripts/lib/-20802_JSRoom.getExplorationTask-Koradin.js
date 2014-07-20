//Stores and calls exploration zones for quests
//Koradin - May 2010
function getExplorationTask( rVnum )
{
	var total_areas = [];
	//Create an area for exploration here: [ bottom_of_zone, task_name ]
	total_areas.push( [2400,"explore southern Aridhol"] );
	total_areas.push( [8000,"explore the trolloc Keep"] );
	total_areas.push( [8500,"explore the plains east of the Keep"] );
	total_areas.push( [8500,"explore the Long Stone Road"] );
	total_areas.push( [8100,"explore the trolloc Stronghold"] );
	total_areas.push( [2000,"explore the northern Blight"] );
	total_areas.push( [1800,"explore the southern Blight"] );
	total_areas.push( [20600,"explore Tarwin's Gap"] );
	total_areas.push( [6400,"explore the northern Fal Moran road"] );
	total_areas.push( [1700,"explore the city of Fal Dara"] );
	total_areas.push( [10300,"explore the city of Fal Moran"] );
	total_areas.push( [9300,"explore the southern Fal Moran road"] );
	total_areas.push( [18100,"explore southern Niamh Pass"] );
	total_areas.push( [6600,"explore northern Niamh Pass"] );
	total_areas.push( [1900,"explore the northern Tar Valon road"] );
	total_areas.push( [1200,"explore the path northwest of Tar Valon"] );
	total_areas.push( [8200,"explore the plains west of Tar Valon"] );
	total_areas.push( [22300,"explore the White Tower"] );
	total_areas.push( [21300,"explore the city of Tar Valon"] );
	total_areas.push( [21700,"explore the town of Aringill"] );
	total_areas.push( [21400,"explore the road east of Caemlyn"] );
	total_areas.push( [3100,"explore the forest south of Caemlyn"] );
	total_areas.push( [300,"explore the road west of Caemlyn"] );
	total_areas.push( [100,"explore the city of Caemlyn"] );
	total_areas.push( [3100,"explore the hills far south of Caemlyn"] );
	total_areas.push( [400,"explore the northern Lugard road"] );
	total_areas.push( [1400,"explore the plains of Murandy"] );
	total_areas.push( [20100,"explore the city of Lugard"] );
	total_areas.push( [10000,"explore the road east of Whitebridge"] );
	total_areas.push( [10100,"explore the road west of Whitebridge"] );
	total_areas.push( [10200,"explore the road east of Baerlon"] );
	total_areas.push( [4500,"explore the town of Baerlon"] );
	total_areas.push( [5300,"explore the forest north of Whitebridge"] );
	//
	for (var _autoKey in total_areas) {
		var area = total_areas[_autoKey];
		if (getRoom(area[0]))
		{
			//if (getRoom(area[0]).zoneIsOpen() == true)
			//{
				for (var i = area[0]; i < (area[0]+100); i++)
				{
					if (getRoom(i))
					{
						if (getRoom(i).isFlagged(constants.ROOM_NOPORT) == false)
							area.push(i);
					}
				}
				var zone_bot = area.shift();
				var result = area.shift();
				area.sort();
				if (arrContains(area,rVnum) == true)
					return [result,area.length,getRoom(zone_bot)];
			//}
		}
	}
	return [0,0,0];
}
