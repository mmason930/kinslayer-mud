function he_she( sex ) 
{ 
    var he_she = ["it", "he", "she"]; 
    if ( sex < 0 || sex >= he_she.length )
    {
        return undefined;
    }
    return he_she[sex];
}
