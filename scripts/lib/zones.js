function getAllRoomsInZone(zoneVnum, predicate) {
    if (!zoneVnum)
        return [];
    let roomsInZone = [];
    let bottomRoomVnum = getZoneBottomRoomVnum(zoneVnum);
    let topRoomVnum = getZoneTopRoomVnum(zoneVnum);

    for(let roomVnum = bottomRoomVnum;roomVnum < topRoomVnum;++roomVnum) {
        let room = getRoom(roomVnum);
        if(!room) {
            continue;
        }
        if(!predicate || predicate(room)) {
            roomsInZone.push(room);
        }
    }

    return roomsInZone;
}