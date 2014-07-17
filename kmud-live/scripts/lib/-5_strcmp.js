//Takes two strings. Returns true if both are identical.
//Does not take character casing into consideration.
function str_cmp(str1, str2)
{
    return !((str1 != undefined) && (str2 != undefined) && (str1.toLowerCase() == str2.toLowerCase()))
}
