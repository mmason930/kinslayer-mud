JSCharacter.prototype.he_she = function()
{ 
    var he_she = ["it", "he", "she"]; 
    if ( this.sex < 0 || this.sex >= he_she.length )
    {
        return undefined;
    }
    return he_she[this.sex];
}
