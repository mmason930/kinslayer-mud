// Sends a message to everyone in the room
// except the character in ignore
function echoaround(ignore, message)
{
    var room = ignore.room;
    var people = room.people;
    var fail;
    for (var _autoKey in people) {
    	var person = people[_autoKey];
        fail = false;
        if (ignore == person)
			fail = true;
        
        if (!fail)
        {
            person.send(message);
        }
    }
}
