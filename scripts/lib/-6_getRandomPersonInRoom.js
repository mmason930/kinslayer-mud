function getRandomPersonInRoom(room)
{
    if( room.people.length == 0 )
        return null;
    else
        return room.people[random(0,room.people.length-1)];
}
