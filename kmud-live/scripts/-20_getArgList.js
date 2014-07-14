/*Given a string args; return an array of each element of the string
	Example: 	'pick door east' -> vArgs = {"pick","door","east"}*/
function getArgList( args ) {
    var arg_list = args.split(" ");
    var vArgs = [];
    for(var i = 0;i < arg_list.length;++i) {
        if( arg_list[i].length > 0 ) {
            vArgs.push( arg_list[i] );
        }
    }
    return vArgs;
}
