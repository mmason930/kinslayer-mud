function clone(obj){
    if(obj == null || typeof(obj) != 'object')
        return obj;
    var temp = obj.constructor(); // changed
    for(var key in obj)
        temp[key] = clone(obj[key]);
    return temp;
}
