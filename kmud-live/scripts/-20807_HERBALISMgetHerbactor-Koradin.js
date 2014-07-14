//Herbalism - Returns a plant for the actor to gather
//Koradin - May 2010
function getHerb( actor )
{
	var roll = random(1,100);
	var sect = actor.room.sector;
	var field = constants.SECT_FIELD;
	var forest = constants.SECT_FOREST;
	var mount = constants.SECT_MOUNTAIN;
	var hills = constants.SECT_HILLS;
	var road = constants.SECT_ROAD;
	if (sect == mount){ var type = 1; }
	else if (sect == hills){ var type = 2; }
	else if (sect == forest){ var type = 3; }
	else if (sect == field){ var type = 4; }
	else if (sect == road){ var type = 5; }
	else{ var type = 5; sendKoradin("herb type defaulted to 5"); }
	var bw = [20986,1,1,3,14,8,"some blackwasp nettles"];
	var bl = [20987,14,14,1,4,7,"some blisterleaf"];
	var gf = [20988,1,1,3,14,8,"some goatflower petals"];
	var ct = [20990,14,14,4,4,7,"some crimsonthorn root"];
	var ff = [20991,14,14,1,4,7,"some five-finger root"];
	var fw = [20992,2,3,23,4,7,"some flatwort leaves"];
	var fr = [20993,1,1,3,14,8,"some forkroot"];
	var gt = [20994,14,14,1,4,7,"some goatstongue"];
	var gw = [20995,3,2,23,4,7,"some greenwort"];
	var gy = [20996,14,13,3,5,7,"some grey fennel"];
	var il = [20997,2,3,23,4,7,"some ivy leaves"];
	var st = [20998,14,14,4,6,7,"some sheepstongue"];
	var sb = [20985,1,1,3,14,8,"some sunburst root"];
	var bk = [20989,5,5,5,5,5,cyn+"a boneknit plant"+nrm];
	var bwc = (100 - bw[type]);
	var blc = (bwc - bl[type]);
	var gfc = (blc - gf[type]);
	var ctc = (gfc - ct[type]);
	var ffc = (ctc - ff[type]);
	var fwc = (ffc - fw[type]);
	var frc = (fwc - fr[type]);
	var gtc = (frc - gt[type]);
	var gwc = (gtc - gw[type]);
 	var gyc = (gwc - gy[type]);
	var ilc = (gyc - il[type]);
	var stc = (ilc - st[type]);
	var sbc = (stc - sb[type]);
	var bkc = (sbc - bk[type]);
	
	if (roll > bwc){
		return [bw[0],bw[6]]; }
	else if (roll > blc){
		return [bl[0],bl[6]]; }
	else if (roll > gfc){
		return [gf[0],gf[6]]; }
	else if (roll > ctc){
		return [ct[0],ct[6]]; }
	else if (roll > ffc){
		return [ff[0],ff[6]]; }
	else if (roll > fwc){
		return [fw[0],fw[6]]; }
	else if (roll > frc){
		return [fr[0],fr[6]]; }
	else if (roll > gtc){
		return [gt[0],gt[6]]; }
	else if (roll > gwc){
		return [gw[0],gw[6]]; }
	else if (roll > gyc){
		return [gy[0],gy[6]]; }
	else if (roll > ilc){ 
		return [il[0],il[6]]; }
	else if (roll > stc){
		return [st[0],st[6]]; }
	else if (roll > sbc){
		return [sb[0],sb[6]]; }
	else if (roll > bkc){
		return [bk[0],bk[6]]; }
	else{
		return null; }
}

