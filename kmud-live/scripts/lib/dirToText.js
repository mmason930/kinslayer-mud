function dirToText( dir ) {
    var dirs = ["north", "east", "south", "west", "up", "down"];
    if( dir < 0 || dir >= dirs.length )
        return undefined;
    return dirs[dir];
}
