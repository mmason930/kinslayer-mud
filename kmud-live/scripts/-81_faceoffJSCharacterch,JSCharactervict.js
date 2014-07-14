JSCharacter.prototype.faceoff = function( vict ) {
	var target;	
	if(!vict.fighting || vict.affectedBy(constants.AFF_GROUP)) {
		return vict;
	}
	for( var i = 0; i < vict.room.people.length; i++ ) {
		target = vict.room.people[i];
		if( (!target.leader && !vict.leader) || !target.affectedBy(constants.AFF_GROUP) ) {
			continue;
		}
		if( this.canSee(target) && (target.leader == vict.leader || target == vict.leader || target.leader == vict) ) {
			if( target.numFighting() < vict.numFighting() && !target.mobFlagged(constants.MOB_MOUNT) && !target.mobFlagged(constants.MOB_SHADOW_MOUNT) ) {
				return target;
			}
		}
	}
	return vict;
}
JSCharacter.prototype.numFighting = function( ) {
	var count = 0;
	var victim;
	for( var i = 0; i < this.room.people.length; i++ ) {
		victim = this.room.people[i];
		if( victim.fighting == this ) {
			count++;
		}
	}
	return count;
}
