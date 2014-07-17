JSCharacter.prototype.his_her = function()
{ 
    var hArr = ["its", "his", "her"]; 
    if ( this.sex < 0 || this.sex >= hArr.length )
    {
        return undefined;
    }
    return hArr[this.sex];
}
