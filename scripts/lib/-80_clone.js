//Taken from http://stackoverflow.com/questions/728360/most-elegant-way-to-clone-a-javascript-object
//Galnro 2014-07-18
function clone(obj) {
	// Use lodash (it supports custom object constructors)
	return _.clone(obj);

	//// Handle the 3 simple types, and null or undefined
	//if (null == obj || "object" != typeof obj) return obj;

	//// Handle Date
	//if (obj instanceof Date) {
	//	var copy = new Date();
	//	copy.setTime(obj.getTime());
	//	return copy;
	//}

	//// Handle Array
	//if (obj instanceof Array) {
	//	var copy = [];
	//	for (var i = 0, len = obj.length; i < len; i++) {
	//		copy[i] = clone(obj[i]);
	//	}
	//	return copy;
	//}

	//// Handle Object
	//if (obj instanceof Object) {
	//	var copy = {};
	//	for (var attr in obj) {
	//		if (obj.hasOwnProperty(attr)) copy[attr] = clone(obj[attr]);
	//	}
	//	return copy;
	//}

	//throw new Error("Unable to copy obj! Its type isn't supported.");
}
/***
function clone(obj){
    if(obj == null || typeof(obj) != 'object')
        return obj;
    var temp = obj.constructor(); // changed
    for(var key in obj)
        temp[key] = clone(obj[key]);
    return temp;
}
***/