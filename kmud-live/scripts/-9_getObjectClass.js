/* Returns the class of the argument or undefined if it's not a valid JavaScript object. */
function getObjectClass(obj)
{
    if (obj && obj.constructor && obj.constructor.toString)
    {
        var arr = obj.constructor.toString().match(/function\s*(\w+)/);
        return arr && arr.length == 2 ? arr[1] : undefined;
    }
    else
    {
        return undefined;
    }
}
