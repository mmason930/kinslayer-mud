/*** Returns the object if it is found in the list. Otherwise, returns null. ***/
function getObjInListVis(ch, objName, list, nr)
{
    if( list.length == 0 )
        return null;
    if( !nr ) {
        var nameList = objName.split(".");
        nr = new Object();
        nr.val = 1;
        if( nameList.length == 2 ) {//3.meat, 2.shield, 5.sword, etc...
            nr.val = parseInt(nameList[0]);
            objName = nameList[1];
            if( nr.val <= 0 )
                return null;
        }
    }
    var type = 0;//Objects
    if( getObjectClass(list[0]) == "Array" )
        type = 1;//List of objects...
    for(var i = 0;i < list.length;i++)
    {
        if( type == 0 ) {
            if( isName(objName, list[i].namelist) && --nr.val <= 0 )
                return list[i];
        }
        else {
            var obj = getObjInListVis(ch, objName, list[i], nr);
            if( obj )
                return obj;
        }
    }
    return null;
}

