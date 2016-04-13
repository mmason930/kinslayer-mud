function getMyShip( room )
{
    if( getRoom(23210).roomInShip( room ) )
        return getRoom(23210);
    return getRoom(23210).getOtherShip();
}
var qwerty;
