JSCharacter.prototype.him_her = function()
{ 
    var him_her = ["it", "him", "her"]; 
    if ( this.sex < 0 || this.sex >= him_her.length )
    {
        return undefined;
    }
    return him_her[this.sex];
}
