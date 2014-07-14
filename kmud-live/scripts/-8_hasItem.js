JSObject.prototype.hasItem = function(item_vnum, recursive)
{
    for(var i in this.contents) {
        if( (getObjectClass(item_vnum) == "Number" && this.contents[i].vnum == item_vnum) )
            return this.contents[i];
		else if( this.contents[i].name == item_vnum )
		    return this.contents[i];//Scanning by name.
        if( recursive ) {
            var obj = this.contents[i].hasItem(item_vnum, true);
            if( obj )
                return obj;
        } 
    }
    return null;
}
