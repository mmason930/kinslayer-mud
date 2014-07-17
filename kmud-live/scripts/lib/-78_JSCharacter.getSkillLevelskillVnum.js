//	June 2010 - Fogel
//	Returns the level of the character's skill
JSCharacter.prototype.getSkillLevel = function( skillName )
{
	return Math.floor( this.getSkill( getSkillVnum(skillName) ) / 12 );
}
