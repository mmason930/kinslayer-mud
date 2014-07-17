// Sends a message to everyone in the room
// except the characters in ignore (an array)
function echoaroundmany(ignore, message)
{
    if (ignore.length <= 0)
        return;
    var room = ignore[0].room;
    var people = room.people;
    var fail;
    for each(var person in people)
    {
        fail = false;
        for each(var ignored in ignore)
        {
            if (ignored == person)
            fail = true;
        }
        if (!fail)
        {
            person.send(message);
        }
    }
}

