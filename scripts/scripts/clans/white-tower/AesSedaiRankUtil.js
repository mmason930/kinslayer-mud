if (typeof (AesSedaiRank) === "undefined")
    AesSedaiRank = {};

/**
 * @namespace AesSedaiRank.Util
 */
AesSedaiRank.Util = (function() {

    let self = {};
    self.mobIndex = {
        22: 22311,
        19: 22308,
        23: 22309,
        20: 22312,
        17: 22314,
        18: 22307,
        21: 22313
    };

    self.portalObjectVnum = 10600;

    self.summonForTest = function(self, actor, here, args) {
        if (!actor.isAesSedai())
            return;
        let vArgs = getArgList(args.toLowerCase());
        if (vArgs.length < 7)
            return;
        if (
            vArgs[0] === "please" &&
            vArgs[1] === "bring" &&
            vArgs[3] === "in" &&
            vArgs[4] === "for" &&
            vArgs[5] === "her" &&
            (vArgs[6] === "test" || vArgs[6] === "test.")
        ) {
            let name = vArgs[2];
            self.comm("narrate " + capFirstLetter(name) + ", you are summoned to be tested for the shawl of an Aes Sedai. The Light keep you whole and see you safe.");
        }
    };

    self.acceptedHasArrived = function(self, actor, here, args) {
        if( !actor.isAesSedai() )
            return;
        let vArgs = getArgList(args.toLowerCase());
        if(
            vArgs[0] === "accepted" &&
            vArgs[2] === "has" &&
            vArgs[3] === "arrived" &&
            vArgs[4] === "for" &&
            vArgs[5] === "her" &&
            (vArgs[6] === "test" || vArgs[6] === "test,") &&
            (vArgs[7] === "sister" || vArgs[7] === "sister.")
        ) {
            let acceptedName = vArgs[1];
            let accepted = getCharInListVis(self, vArgs[1], here.people);

            let targetRoom = getRoom(10574);
            let portalHere;
            let portalThere;

            runSequence([
                [waitPulse(1), function() {
                    if( !accepted ) {
                        self.say("Really? Because I do not see " + capFirstLetter(acceptedName) + " here.");
                        return false;
                    }

                    for (const clanNum in self.mobIndex) {
                        if( !actor.inClan( clanNum ) )
                            targetRoom.loadMob( self.mobIndex[ clanNum ] );
                    }
                }],
                [waitPulse(2), () => {
                    self.say("Now that you are here " + accepted.name + ", let us go to the testing chamber where the others are waiting.");
                }],
                [waitPulse(3), () => {
                    self.comm("embrace");
                }],
                [waitPulse(2), () => {
                    self.comm("emote weaves her hands before her, creating a shimmering portal.");
                    portalHere = here.loadObj(self.portalObjectVnum);
                    portalThere = targetRoom.loadObj(self.portalObjectVnum);
                    targetRoom.echo("A shimmering portal appears.");
                }],
                [waitPulse(4), () => {
                    self.comm("emote motions for " + accepted.name + " to lead the way.");
                }],
                [waitPulse(4), () => {
                    accepted.send("You step through the portal.");
                    act("$n steps into the portal.",false, accepted, null, null, constants.TO_ROOM);
                    accepted.moveToRoom(targetRoom);
                    act("$n steps through the portal.", false, accepted, null, null, constants.TO_ROOM);
                    accepted.comm("look");
                }],
                [waitPulse(3), () => {
                    actor.send("You follow " + accepted.name + " through the portal.");
                    actor.moveToRoom(targetRoom);
                    actor.comm("look");
                    act("$n steps through the portal.", false, actor, null, null, constants.TO_ROOM);
                }],
                [waitPulse(2), () => {
                    portalHere.extract();
                    self.moveToRoom(targetRoom);
                    act("$n steps through the portal.", false, self, null, null, constants.TO_ROOM);
                }],
                [waitPulse(3), () => {
                    portalThere.extract();
                    targetRoom.echo("The portal closes in on itself.");
                }],
                [waitPulse(4), () => {
                    act("A circle is formed around $n and Sheriam Sedai by the Aes Sedai present.", true, accepted, null, null, constants.TO_ROOM);
                    actor.send("You join the circle of women surrounding " + accepted.name + ".");
                    accepted.send("The Aes Sedai draw in to form a circle around you and Sheriam Sedai.");
                }],
                [waitPulse(7), () => {
                    self.say(accepted.name + ", you are here today to test your preparation for the shawl. I hope that you are ready for this test.");
                }],
                [waitPulse(3), () => {
                    self.say("Women have died through that arch.");
                }],
                [waitPulse(2), () => {
                    self.comm("emote gestures to the oval-shaped ring hanging in the air.");
                }],
                [waitPulse(6), () => {
                    act("$N looks sternly at you.", false, accepted, null, self, constants.TO_CHAR);
                    act("$n looks sternly at $N.", false, self, null, accepted, constants.TO_NOTVICT);
                }],
                [waitPulse(2), () => {
                    self.say("You have completed all of the necessary steps up to this point, " + accepted.name + ". Please remove your clothing and hand your ring to me.");
                }]
            ]);
        }
    }

    return self;
})();