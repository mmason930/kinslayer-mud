/*Returns the direction index in the opposite direction of dir
	Example: dir = 0 -> return 2*/
function revDir( dir ) {
    if( dir < 0 || dir >= 6 )
        return -1;
    var rd = [2,3,0,1,5,4];
    return rd[dir];
}
