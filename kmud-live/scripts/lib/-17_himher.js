/*Return "its" "him" or "her" corresponding to their sex*/
function him_her( sex ) 
{ 
    var him_her = ["it", "him", "her"]; 
    if ( sex < 0 || sex >= him_her.length )
    {
        return undefined;
    }
    return him_her[sex];
}
