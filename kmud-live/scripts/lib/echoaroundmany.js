// Sends a message to everyone in the room
// except the characters in ignore (an array)
function echoaroundmany(ignore, message)
{
    if (ignore.length <= 0)
        return;
    var room = ignore[0].room;
    var people = room.people;
    var fail;
    for (var _autoKey in people) {
    	var person = people[_autoKey];
        fail = false;
        for (var _autoKey in ignore) {
        	var ignored = ignore[_autoKey];
            if (ignored == person)
            fail = true;
        }
        if (!fail)
        {
            person.send(message);
        }
    }
}

