JSCharacter.prototype.hasItem =
function(item_vnum, check_inv, check_eq, recursive)
{
    if( check_inv ) {
        for(var i in this.inventory) {
            if( (getObjectClass(item_vnum) == "Number" && this.inventory[i].vnum == item_vnum) )
                return this.inventory[i];
			else if( this.inventory[i].name == item_vnum )
			    return this.inventory[i];
            if( recursive ) {
                var obj = this.inventory[i].hasItem(item_vnum, true);
                if( obj )
                    return obj;
            }
        }
    }
    if( check_eq ) {
        for(var i = 0;i < 22;++i) {
            if( !this.eq(i) )
                continue;
            if( (getObjectClass(item_vnum) == "Number" && this.eq(i).vnum == item_vnum) )
                return this.eq(i);
			else if( this.eq(i).name == item_vnum )
			    return this.eq(i);
            if( recursive ) {
                var obj = this.eq(i).hasItem(item_vnum, true);
                if( obj )
                    return obj;
            }
        }
    }
    return null;
}
