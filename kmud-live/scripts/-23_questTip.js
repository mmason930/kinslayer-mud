JSCharacter.prototype.questSend =
function(msg) {
    var cyn = this.cyan(constants.CL_NORMAL);
    var bld = this.bold(constants.CL_NORMAL);
    var nrm = this.normal(constants.CL_NORMAL);
    this.send( cyn + bld + "[QUEST] : " + msg + nrm);
}
