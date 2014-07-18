JSRow.prototype.getInteger = function(columnName) {
	var value = this.get(columnName);

	if(value == null)
		return null;
	return parseInt(value);
};

JSRow.prototype.getIntBoolean = function(columnName) {

	var value = this.getInt(columnName);

	return value ? true : false;
};

JSRow.prototype.getTimestamp = function(columnName) {
	var timestampString = this.get(columnName);

	if(timestampString == null)
		return null;

	var date = new Date();
	date.setMilliseconds(0);
	if(timestampString.length == 19) {
		date.setFullYear(parseFloat(timestampString.substr(0,4)));
		date.setMonth(parseFloat(timestampString.substr(5,2))-1);
		date.setDate(parseFloat(timestampString.substr(8,2)));
		date.setHours(parseFloat(timestampString.substr(11,2)));
		date.setMinutes(parseFloat(timestampString.substr(14,2)));
		date.setSeconds(parseFloat(timestampString.substr(17,2)));
		return date;
	}
	if(timestampString.length == 10) {
		date.setFullYear(parseFloat(timestampString.substr(0,4)));
		date.setMonth(parseFloat(timestampString.substr(5,2)));
		date.setDate(parseFloat(timestampString.substr(7,2)));
		date.setHours(0);
		date.setMinutes(0);
		date.setSeconds(0);
		return date;
	}

	return null;
};