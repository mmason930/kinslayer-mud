function encodeToHex(str){
	var r="";
	var e=str.length;
	var c=0;
	var h;
	while(c<e){
		h=str.charCodeAt(c++).toString(16);
		while(h.length<3) h="0"+h;
		r+=h;
	}
	return r;
}
function decodeFromHex(str){
	var r="";
	var e=str.length;
	var s;
	while(e>=0){
		s=e-3;
		r=String.fromCharCode("0x"+str.substring(s,e))+r;
		e=s;
	}
	return r;
}
