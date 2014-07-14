/*Return "its" "his" or "her" corresponding to their sex*/
function his_her( sex ) 
{ 
    var his_her = ["its", "his", "her"]; 
    if ( sex < 0 || sex >= his_her.length )
    {
        return undefined;
    }
    return his_her[sex];
}
