/**
 * @param {string} sqlStr The SQL query string with argument placeholders in the form of {#},
 * where # corresponds to an argument provided in args
 * @param {...mixed} args Additional arguments which will be escaped and injected into the query string
 * @returns The provided sqlStr with escaped args inserted into the proper argument placeholders.
 */
function prepareStatement(sqlStr, args) {
	// Exclude sqlStr from args
	var args = _.rest(arguments);
	// Get the escaped version of all args
	var escapedArgs = _.map(args, function(a) { return sqlEsc(a); });
	// Fill placeholders in sqlStr with escaped args
	return String.format.apply(String, [sqlStr].concat(escapedArgs));
};