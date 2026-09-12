if (typeof (AesSedaiRank) === "undefined")
    AesSedaiRank = {};

/**
 * @namespace AesSedaiRank.Util
 *
 * Contains all logic for the Accepted-to-Aes Sedai shawl test quest.
 * Entry point scripts in aes-sedai-ranking/ are thin wrappers that call into these functions.
 */
AesSedaiRank.Util = (function() {

    let self = {};

    // =========================================================================
    // Constants
    // =========================================================================

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
    self.testingChamberVnum = 10574;
    self.acceptedRingVnum = 815;

    self.testRoomVnums = [null, 10576, 10577, 10578, 10579, 10580];
    self.weaveOrder = ["air", "fire", "water", "earth", "spirit"];

    self.ajahMap = {
        "red":    constants.CLAN_RED_AJAH,
        "green":  constants.CLAN_GREEN_AJAH,
        "brown":  constants.CLAN_BROWN_AJAH,
        "yellow": constants.CLAN_YELLOW_AJAH,
        "blue":   constants.CLAN_BLUE_AJAH,
        "white":  constants.CLAN_WHITE_AJAH,
        "gray":   constants.CLAN_GRAY_AJAH
    };

    // Reverse map: clan number -> color name
    self.ajahNames = {};
    for (let color in self.ajahMap) {
        self.ajahNames[self.ajahMap[color]] = color;
    }

    // =========================================================================
    // Quest State (lives on the Util object, reset between ceremonies)
    // =========================================================================

    self.accepted = null;       // Character ref: the Accepted being tested
    self.acceptedRing = null;   // Object ref: her own ring, held by Sheriam until she returns
    self.summoner = null;       // Character ref: the Aes Sedai who initiated
    self.sheriam = null;        // Character ref: the Sheriam mob instance
    self.currentTest = 0;       // Which test (1-5), 0 = not in test
    self.onStar = false;        // Whether player is standing on the star
    self.starRevealed = false;  // Whether the star has appeared in current test
    self.testPhase = "idle";    // "idle" | "ceremony" | "testing" | "ajahSelect"

    // =========================================================================
    // Helpers
    // =========================================================================

    self.getTestNumberForRoom = function(roomVnum) {
        for (let i = 1; i <= 5; i++) {
            if (self.testRoomVnums[i] === roomVnum) return i;
        }
        return 0;
    };

    self.resetState = function() {
        self.accepted = null;
        self.acceptedRing = null;
        self.summoner = null;
        self.sheriam = null;
        self.currentTest = 0;
        self.onStar = false;
        self.starRevealed = false;
        self.testPhase = "idle";
    };

    self.sheriamHomeVnum = 22344;

    self.cleanupTestRooms = function() {
        for (let i = 1; i <= 5; i++) {
            let room = getRoom(self.testRoomVnums[i]);
            if (room) {
                room.getCharacters(function(c) { return c.isMob(); })
                    .forEach(function(mob) { mob.extract(true); });
                let items = room.items;
                for (let j = items.length - 1; j >= 0; j--) {
                    items[j].extract();
                }
            }
        }
    };

    self.cleanupTestingChamber = function() {
        let chamber = getRoom(self.testingChamberVnum);
        if (!chamber) return;

        // Extract all mobs EXCEPT Sheriam (she gets moved home)
        let sheriam = self.sheriam;
        chamber.getCharacters(function(c) { return c.isMob(); })
            .forEach(function(mob) {
                if (mob === sheriam) return;
                mob.extract(true);
            });

        // Extract leftover items (portals, etc.)
        let items = chamber.items;
        for (let j = items.length - 1; j >= 0; j--) {
            items[j].extract();
        }
    };

    self.sendEveryoneHome = function() {
        let sheriamHome = getRoom(self.sheriamHomeVnum);
        let sheriam = self.sheriam;
        let accepted = self.accepted;
        let summoner = self.summoner;

        if (sheriam) {
            sheriam.moveToRoom(sheriamHome);
        }
        if (accepted) {
            accepted.moveToRoom(sheriamHome);
            accepted.comm("look");
        }
        if (summoner) {
            summoner.moveToRoom(sheriamHome);
            summoner.comm("look");
        }
    };

    //Returns a message explaining why this character may not be tested, or null when she may.
    self.getIneligibilityReason = function(candidate) {
        const acceptedRank = 3;
        const aesSedaiRank = 5;

        if (!candidate.inClan(constants.CLAN_WHITE_TOWER))
            return candidate.name + " is not a member of the White Tower.";

        let rank = candidate.getRank(constants.CLAN_WHITE_TOWER);

        if (rank < acceptedRank)
            return candidate.name + " has not yet been raised to Accepted. She is not ready for this test.";

        if (rank >= aesSedaiRank)
            return candidate.name + " already wears the shawl.";

        //She is ranked once for every step up to Aes Sedai, and the last step is the most
        //expensive, so check the whole climb now rather than stalling at the raising.
        let questPoints = candidate.getClanQuestPoints(constants.CLAN_WHITE_TOWER);
        let required = getRankRequirement(aesSedaiRank);

        //Never let a missing value quietly disable the check - that is how a candidate ends up
        //walking the entire test only to not be raised.
        if (typeof questPoints !== "number" || typeof required !== "number") {
            mudLog(constants.BRF, 102, "Aes Sedai raising: quest point check skipped - questPoints=" + questPoints + " required=" + required);
        }
        else if (questPoints < required) {
            return candidate.name + " has not earned enough quest points to be raised. She needs " + required + ", and has " + questPoints + ".";
        }

        return null;
    };

    //Give her back the ring she handed over. Falls back to a fresh one only if the original
    //has gone missing, so she is never left without it.
    self.returnRing = function(accepted) {
        let ring = self.acceptedRing;

        self.acceptedRing = null;

        if (ring) {
            ring.moveToChar(accepted);
            return;
        }

        accepted.loadObj(self.acceptedRingVnum);
    };

    self.extractPortalFromRoom = function(room) {
        let items = room.items;
        for (let i = 0; i < items.length; i++) {
            if (items[i].vnum === self.portalObjectVnum) {
                items[i].extract();
                return;
            }
        }
    };

    // =========================================================================
    // Phase 1: Summoning
    // Script 22802 - Speech trigger on Sheriam
    // An Aes Sedai says: "please bring [name] in for her test"
    // =========================================================================

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

    // =========================================================================
    // Phase 2: Arrival & Ceremony
    // Script 22803 - Speech trigger on Sheriam
    // An Aes Sedai says: "accepted [name] has arrived for her test, sister"
    // =========================================================================

    self.acceptedHasArrived = function(self, actor, here, args) {
        var Util = AesSedaiRank.Util;

        if (!actor.isAesSedai())
            return;
        let vArgs = getArgList(args.toLowerCase());
        if (
            vArgs[0] === "accepted" &&
            vArgs[2] === "has" &&
            vArgs[3] === "arrived" &&
            vArgs[4] === "for" &&
            vArgs[5] === "her" &&
            (vArgs[6] === "test" || vArgs[6] === "test,") &&
            (vArgs[7] === "sister" || vArgs[7] === "sister.")
        ) {
            let acceptedName = vArgs[1];
            let accepted = getCharInListVis(self, acceptedName, here.people);

            //Validate the candidate before any state is stored, any sister is loaded or any
            //portal is opened, and tell the sister who presented her exactly what is wrong.
            //Without this a stranger can walk the whole test and be handed an Ajah shawl, and
            //a candidate short on quest points reaches the end and is silently not raised.
            if (accepted) {
                let rejection = Util.getIneligibilityReason(accepted);

                if (rejection) {
                    self.say(rejection);
                    return;
                }
            }

            let targetRoom = getRoom(Util.testingChamberVnum);
            let portalHere;
            let portalThere;

            // Store ceremony state
            Util.summoner = actor;
            Util.sheriam = self;
            Util.testPhase = "ceremony";

            runSequence([
                [waitPulse(1), function() {
                    if (!accepted) {
                        self.say("Really? Because I do not see " + capFirstLetter(acceptedName) + " here.");
                        Util.resetState();
                        return false;
                    }

                    Util.accepted = accepted;

                    for (const clanNum in Util.mobIndex) {
                        if (!actor.inClan(clanNum))
                            targetRoom.loadMob(Util.mobIndex[clanNum]);
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
                    portalHere = here.loadObj(Util.portalObjectVnum);
                    portalThere = targetRoom.loadObj(Util.portalObjectVnum);
                    targetRoom.echo("A shimmering portal appears.");
                }],
                [waitPulse(4), () => {
                    self.comm("emote motions for " + accepted.name + " to lead the way.");
                }],
                [waitPulse(4), () => {
                    accepted.send("You step through the portal.");
                    act("$n steps into the portal.", false, accepted, null, null, constants.TO_ROOM);
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
    };

    // =========================================================================
    // Phase 3: Ring & Instructions
    // Script 22804 - Character Receive trigger on Sheriam
    // Accepted gives ring (vnum 815) to Sheriam
    // =========================================================================

    self.ringGiven = function(self, actor, here, args, extra) {
        var Util = AesSedaiRank.Util;
        var ring = extra.obj;

        if (ring.vnum !== Util.acceptedRingVnum)
            return;
        if (Util.testPhase !== "ceremony")
            return;

        let accepted = Util.accepted;

        //Hold on to her own ring rather than minting a replacement later.
        Util.acceptedRing = ring;

        runSequence([
            [waitPulse(2), function() {
                self.comm("emote nods and drops the ring into a pocket of her dress.");
            }],
            [waitPulse(3), () => {
                self.say("I'll keep this safe with me until you return.");
            }],
            [waitPulse(5), () => {
                self.say("When you step through the ter'angreal, you will no longer be in this world.");
            }],
            [waitPulse(3), () => {
                here.echo("You feel a strange sensation from somewhere nearby.");
            }],
            [waitPulse(2), () => {
                here.echo("An Aes Sedai steps forward and says 'Remember what must be remembered.'");
            }],
            [waitPulse(1), () => {
                accepted.send("Your skin tingles as a weave is cast over you.");
                act("$n shivers as a weave is cast over her.", false, accepted, null, null, constants.TO_ROOM);
            }],
            [waitPulse(2), () => {
                here.echo("A strange sensation from nearby gets weaker.");
            }],
            [waitPulse(4), () => {
                self.say("But every scrape and cut you receive in that other world will be just as real as in ours.");
            }],
            [waitPulse(3), () => {
                here.echo("You feel a strange sensation from somewhere nearby.");
            }],
            [waitPulse(2), () => {
                here.echo("An Aes Sedai steps forward and says 'Remember what must be remembered.'");
            }],
            [waitPulse(1), () => {
                accepted.send("Your skin tingles as a weave is cast over you.");
                act("$n shivers as a weave is cast over her.", false, accepted, null, null, constants.TO_ROOM);
            }],
            [waitPulse(2), () => {
                here.echo("A strange sensation from nearby gets weaker.");
            }],
            [waitPulse(5), () => {
                self.say("You will find a six-pointed star in each trial. Stand upon it and weave the correct element to move forward.");
            }],
            [waitPulse(4), () => {
                self.say("Remember this order: air, fire, water, earth, spirit.");
            }],
            [waitPulse(5), () => {
                self.say("That is the order in which you will be weaving.");
            }],
            [waitPulse(3), () => {
                here.echo("You feel a strange sensation from somewhere nearby.");
            }],
            [waitPulse(2), () => {
                here.echo("An Aes Sedai steps forward and says 'Remember what must be remembered.'");
            }],
            [waitPulse(1), () => {
                accepted.send("Your skin tingles as a weave is cast over you.");
                act("$n shivers as a weave is cast over her.", false, accepted, null, null, constants.TO_ROOM);
            }],
            [waitPulse(2), () => {
                here.echo("A strange sensation from nearby gets weaker.");
            }],
            [waitPulse(4), () => {
                self.say("There will be distractions to keep you from your duties. Do not let this hinder you.");
            }],
            [waitPulse(3), () => {
                here.echo("You feel a strange sensation from somewhere nearby.");
            }],
            [waitPulse(2), () => {
                here.echo("An Aes Sedai steps forward and says 'Remember what must be remembered.'");
            }],
            [waitPulse(1), () => {
                accepted.send("Your skin tingles as a weave is cast over you.");
                act("$n shivers as a weave is cast over her.", false, accepted, null, null, constants.TO_ROOM);
            }],
            [waitPulse(2), () => {
                here.echo("A strange sensation from nearby gets weaker.");
            }],
            [waitPulse(4), () => {
                self.say("Lastly, tell no one what has been said here today or of your journey through the ter'angreal.");
            }],
            [waitPulse(5), () => {
                accepted.send("The Aes Sedai surrounding you begin weaving a unanimous pattern with their hands, channeling all Five Powers into the ter'angreal.");
                act("The Aes Sedai around $n begin weaving their hands in a complicated pattern, channeling all Five Powers into the ter'angreal.", false, accepted, null, null, constants.TO_ROOM);
            }],
            [waitPulse(4), () => {
                here.echo("The ter'angreal begins to revolve slowly and a portal of shimmering color can be seen through the center of the oval.");
            }],
            [waitPulse(5), () => {
                self.say("Put your armor back on, " + accepted.name + ". It is dangerous through the portal.");
            }],
            [waitPulse(1), () => {
                accepted.send("Type 'enter portal' to begin your test.");
            }]
        ]);
    };

    // =========================================================================
    // Phase 4: Enter the Ter'angreal
    // Script 22806 - Command trigger "enter" on testing chamber (room 10574)
    // =========================================================================

    self.enterPortal = function(self, actor, here, args) {
        var Util = AesSedaiRank.Util;

        if (Util.testPhase !== "ceremony" || !Util.accepted)
            return false;
        if (actor !== Util.accepted) {
            actor.send("The ter'angreal hums with power, but it is not meant for you.");
            return "blocked";
        }

        let targetRoom = getRoom(Util.testRoomVnums[1]);

        Util.currentTest = 1;
        Util.testPhase = "testing";
        Util.onStar = false;
        Util.starRevealed = false;

        act("$n steps into the ter'angreal and disappears.", false, actor, null, null, constants.TO_ROOM);
        actor.send("You step into the ter'angreal.");
        actor.moveToRoom(targetRoom);
        actor.comm("inroom");

        return true;
    };

    // =========================================================================
    // Phase 5: The Five Tests
    // Scripts 22807-22811 - Command "inroom" triggers on test rooms 10576-10580
    // =========================================================================

    self.testRoomEnter = function(self, actor, here, args, testNum) {
        var Util = AesSedaiRank.Util;

        if (actor !== Util.accepted) return;
        if (Util.testPhase !== "testing") return;

        Util.currentTest = testNum;
        Util.onStar = false;
        Util.starRevealed = false;

        let narrative = Util.narratives[testNum];
        if (narrative) {
            narrative(actor, here);
        }
    };

    // -------------------------------------------------------------------------
    // Test Narratives (each called from testRoomEnter)
    // -------------------------------------------------------------------------

    self.narratives = {};

    // Test 1: Air - The Windswept Pass
    self.narratives[1] = function(actor, here) {
        var Util = AesSedaiRank.Util;
        runSequence([
            [waitPulse(2), function() {
                actor.comm("look");
            }],
            [waitPulse(3), () => {
                Util.extractPortalFromRoom(here);
                here.echo("The portal closes behind you, swallowed by the howling wind.");
            }],
            [waitPulse(4), () => {
                here.echo("The wind tears at you, nearly driving you to your knees.");
            }],
            [waitPulse(5), () => {
                here.echo("A scream cuts through the gale. On the ledge below, a young girl clings to the rock face, her fingers white-knuckled against the stone.");
            }],
            [waitPulse(4), () => {
                actor.send("Her eyes find yours. 'Please!' she cries. 'I can't hold on!'");
            }],
            [waitPulse(6), () => {
                here.echo("The wind intensifies, and you see her grip begin to slip. One hand tears free, leaving her dangling by a single arm.");
            }],
            [waitPulse(5), () => {
                actor.send("You could weave a cushion of Air beneath her. It would be so easy. She is going to die.");
            }],
            [waitPulse(4), () => {
                actor.send("Remember what must be remembered.");
            }],
            [waitPulse(6), () => {
                here.echo("Her sobbing is nearly lost in the wind. Her fingers are sliding, one by one.");
            }],
            [waitPulse(5), () => {
                here.echo("Through your tear-blurred eyes, you notice a large six-pointed star etched into the stone beneath your feet, glowing faintly.");
                Util.starRevealed = true;
            }]
        ]);
    };

    // Test 2: Fire - The Burning Corridor
    self.narratives[2] = function(actor, here) {
        var Util = AesSedaiRank.Util;
        runSequence([
            [waitPulse(2), function() {
                actor.comm("look");
            }],
            [waitPulse(3), () => {
                here.echo("The world reforms around you in heat and smoke and screaming flame.");
            }],
            [waitPulse(5), () => {
                here.echo("A section of ceiling collapses ahead, sending a shower of sparks cascading across the floor. Through the smoke, you make out a figure pinned beneath a fallen beam.");
            }],
            [waitPulse(5), () => {
                here.echo("'Help me!' A novice, barely more than a girl, struggles beneath the heavy timber. Flames creep closer to her white dress.");
            }],
            [waitPulse(4), () => {
                actor.send("You could weave Fire, push the flames back, burn through the beam. She looks at you with terrified eyes.");
            }],
            [waitPulse(6), () => {
                here.echo("The flames reach the hem of her dress. She screams, batting at the fire with her free hand.");
            }],
            [waitPulse(5), () => {
                actor.send("Remember what must be remembered.");
            }],
            [waitPulse(5), () => {
                here.echo("The smoke parts for a moment, and on the scorched marble floor you see the outline of a six-pointed star, glowing with a cool light that the fire cannot touch.");
                Util.starRevealed = true;
            }]
        ]);
    };

    // Test 3: Water - The Flooded Riverbank
    self.narratives[3] = function(actor, here) {
        var Util = AesSedaiRank.Util;
        runSequence([
            [waitPulse(2), function() {
                actor.comm("look");
            }],
            [waitPulse(3), () => {
                here.echo("You are standing in rain so heavy you can barely see. The river roars past, a wall of churning brown water.");
            }],
            [waitPulse(5), () => {
                here.echo("A woman stands at the water's edge, screaming and reaching toward the river. 'My child! Someone save my child!'");
            }],
            [waitPulse(4), () => {
                here.echo("In the current, a small dark head bobs above the surface, tiny arms flailing. The child is being swept downstream.");
            }],
            [waitPulse(5), () => {
                actor.send("You could weave Water, part the current, lift the child free. The mother falls to her knees in the mud, wailing.");
            }],
            [waitPulse(6), () => {
                here.echo("The child goes under. Surfaces. Goes under again. Each time, a little farther away.");
            }],
            [waitPulse(5), () => {
                actor.send("Remember what must be remembered.");
            }],
            [waitPulse(5), () => {
                here.echo("The rain slackens for just a moment, and there on the muddy bank, a six-pointed star gleams as though lit from beneath the earth.");
                Util.starRevealed = true;
            }]
        ]);
    };

    // Test 4: Earth - The Collapsed Mineshaft
    self.narratives[4] = function(actor, here) {
        var Util = AesSedaiRank.Util;
        runSequence([
            [waitPulse(2), function() {
                actor.comm("look");
            }],
            [waitPulse(3), () => {
                here.echo("Dust and silence. The ground shudders beneath your feet, and somewhere deep below, stone grinds against stone.");
            }],
            [waitPulse(5), () => {
                here.echo("An old man kneels in the rubble, bleeding from a gash on his forehead. His hands are torn and raw from digging. 'My family,' he rasps. 'They're trapped below. I can hear them.'");
            }],
            [waitPulse(4), () => {
                here.echo("Faintly, muffled by tons of rock, you hear voices calling. Children's voices.");
            }],
            [waitPulse(5), () => {
                actor.send("You could weave Earth. You could move the stone, open a passage. The old man looks at you with desperate, knowing eyes. 'You can channel, can't you? Please. They'll suffocate.'");
            }],
            [waitPulse(6), () => {
                here.echo("The ground trembles again. The muffled voices go quiet for a long, terrible moment, then resume, weaker now.");
            }],
            [waitPulse(5), () => {
                actor.send("Remember what must be remembered.");
            }],
            [waitPulse(5), () => {
                here.echo("As the dust settles, you see it clearly -- a six-pointed star, scratched deep into the exposed bedrock, pulsing with a faint inner light.");
                Util.starRevealed = true;
            }]
        ]);
    };

    // Test 5: Spirit - The Sunlit Room
    self.narratives[5] = function(actor, here) {
        var Util = AesSedaiRank.Util;
        runSequence([
            [waitPulse(2), function() {
                actor.comm("look");
            }],
            [waitPulse(3), () => {
                here.echo("The world changes. There is no violence here, no danger. Only warmth, and light, and peace.");
            }],
            [waitPulse(6), () => {
                here.echo("A man sits by the fire, handsome and gentle-eyed. He looks up as you appear and smiles. 'There you are,' he says, as though he has been waiting for you all his life.");
            }],
            [waitPulse(5), () => {
                here.echo("'Come sit with me,' he says. 'The children will be home soon. Stay. You have earned your rest.'");
            }],
            [waitPulse(6), () => {
                actor.send("He takes your hand, and his touch is warm. 'You don't have to go back,' he says softly. 'No more tests. No more sacrifice. Just this. Just us.'");
            }],
            [waitPulse(5), () => {
                here.echo("Through the window, you can see children running in a sunlit field. Your children. The life you might have had.");
            }],
            [waitPulse(6), () => {
                actor.send("Everything you have ever wanted is here. Everything you have ever given up.");
            }],
            [waitPulse(5), () => {
                actor.send("Remember what must be remembered.");
            }],
            [waitPulse(6), () => {
                here.echo("On the floor between you and the hearth, barely visible through the pattern of the rug, a six-pointed star shimmers into view, faint but unmistakable.");
                Util.starRevealed = true;
            }],
            [waitPulse(5), () => {
                here.echo("'Please don't go,' he whispers. 'If you leave, I will be gone forever. And you will never have this again.'");
            }]
        ]);
    };

    // =========================================================================
    // Stand on Star
    // Script 22840 - Command trigger "stand" on all 5 test rooms
    // =========================================================================

    self.standStar = function(self, actor, here, args) {
        var Util = AesSedaiRank.Util;

        if (actor !== Util.accepted) return;
        if (Util.testPhase !== "testing") return;

        if (!Util.starRevealed) {
            actor.send("Remember what must be remembered.");
            return;
        }

        if (actor.position !== constants.POS_STANDING) {
            actor.send("You aren't even on your feet!");
            return;
        }

        if (Util.onStar) {
            actor.send("You are already standing on the star.");
            return;
        }

        Util.onStar = true;
        actor.send("You step into the center of the six-pointed star. It glows softly beneath your feet.");
        act("$n steps into the center of a glowing six-pointed star.", false, actor, null, null, constants.TO_ROOM);
        actor.send("You must now weave. Remember what must be remembered.");
    };

    // =========================================================================
    // Weave Element
    // Script 22841 - Command trigger "weave" on all 5 test rooms
    // =========================================================================

    self.weaveElement = function(self, actor, here, args) {
        var Util = AesSedaiRank.Util;

        if (actor !== Util.accepted) return;
        if (Util.testPhase !== "testing") return;

        let vArgs = getArgList(args.toLowerCase());
        if (vArgs.length < 2) {
            actor.send("You must weave one of the Five Powers: air, fire, water, earth, or spirit.");
            return;
        }

        let element = vArgs[1];

        if (!Util.onStar) {
            actor.send("You must stand on the star first.");
            return;
        }

        let expectedElement = Util.weaveOrder[Util.currentTest - 1];

        if (element !== expectedElement) {
            // Wrong element -- test fails
            Util.onStar = false;
            Util.starRevealed = false;

            runSequence([
                [waitPulse(1), function() {
                    actor.send("You reach for saidar and weave threads of " + capFirstLetter(element) + ".");
                }],
                [waitPulse(3), () => {
                    actor.send("The weave unravels. Agony lances through you as the ter'angreal rejects the weave. You chose wrongly.");
                    act("$n cries out in pain as light flares angrily from the star.", false, actor, null, null, constants.TO_ROOM);
                }],
                [waitPulse(4), () => {
                    here.echo("The world dissolves around you. You feel yourself falling...");
                }],
                [waitPulse(3), () => {
                    Util.testFailed();
                }]
            ]);
            return;
        }

        // Correct element -- advance
        Util.onStar = false;
        Util.starRevealed = false;

        let elementName = capFirstLetter(element);

        if (Util.currentTest < 5) {
            // Tests 1-4: transition to next test room
            let nextRoom = getRoom(Util.testRoomVnums[Util.currentTest + 1]);

            runSequence([
                [waitPulse(1), function() {
                    actor.send("You embrace saidar and weave threads of " + elementName + ". The weave settles over the star, which blazes with light.");
                }],
                [waitPulse(3), () => {
                    here.echo("Brilliant light erupts from the six-pointed star, consuming everything.");
                }],
                [waitPulse(3), () => {
                    actor.send("The world dissolves into blinding white light...");
                }],
                [waitPulse(3), () => {
                    actor.moveToRoom(nextRoom);
                    actor.comm("inroom");
                }]
            ]);
        } else {
            // Test 5: return to testing chamber
            runSequence([
                [waitPulse(1), function() {
                    actor.send("You embrace saidar and weave threads of " + elementName + ". The weave completes, and brilliant light erupts from the star.");
                }],
                [waitPulse(3), () => {
                    here.echo("The man's form fades like morning mist. 'Remember what must be remembered,' you whisper to yourself.");
                }],
                [waitPulse(4), () => {
                    here.echo("The light consumes everything. The room, the warmth, the life that might have been -- all of it dissolves.");
                }],
                [waitPulse(4), () => {
                    actor.send("You feel yourself pulled through the blinding light...");
                    let chamber = getRoom(Util.testingChamberVnum);
                    actor.moveToRoom(chamber);
                }],
                [waitPulse(3), () => {
                    Util.testComplete();
                }]
            ]);
        }
    };

    // =========================================================================
    // Test Failed
    // Called when the player weaves the wrong element
    // =========================================================================

    self.testFailed = function() {
        var Util = AesSedaiRank.Util;
        let accepted = Util.accepted;
        let sheriam = Util.sheriam;
        let chamber = getRoom(Util.testingChamberVnum);

        accepted.moveToRoom(chamber);

        runSequence([
            [waitPulse(2), function() {
                accepted.send("You stumble out of the ter'angreal, pale and shaken.");
                act("$n stumbles out of the ter'angreal, pale and shaken.", false, accepted, null, null, constants.TO_ROOM);
            }],
            [waitPulse(4), () => {
                if (sheriam) {
                    sheriam.say("It is unfortunate. You were not ready, " + accepted.name + ". Perhaps in time, you may try again.");
                }
                //She failed the test, not the Tower - her ring is still hers.
                Util.returnRing(accepted);
            }],
            [waitPulse(6), () => {
                if (sheriam) {
                    sheriam.say("Come. Let us return.");
                }
            }],
            [waitPulse(4), () => {
                Util.cleanupTestRooms();
                Util.cleanupTestingChamber();
                Util.sendEveryoneHome();
                Util.resetState();
            }]
        ]);
    };

    // =========================================================================
    // Test Complete (all 5 tests passed)
    // Called after the player successfully weaves Spirit in test 5
    // =========================================================================

    self.testComplete = function() {
        var Util = AesSedaiRank.Util;
        let accepted = Util.accepted;
        let summoner = Util.summoner;
        let sheriam = Util.sheriam;
        let chamber = getRoom(Util.testingChamberVnum);

        Util.testPhase = "ajahSelect";
        Util.currentTest = 0;

        runSequence([
            [waitPulse(3), function() {
                accepted.comm("look");
            }],
            [waitPulse(4), () => {
                accepted.send("You step from the ter'angreal, intact and radiant with the glow of saidar.");
                act("$n steps from the ter'angreal, intact and radiant with the glow of saidar.", false, accepted, null, null, constants.TO_ROOM);
            }],
            [waitPulse(6), () => {
                if (sheriam) {
                    sheriam.say("It is done. You have passed the test, " + accepted.name + ".");
                }
            }],
            [waitPulse(4), () => {
                chamber.echo("A murmur of approval passes through the assembled sisters.");
            }],
            [waitPulse(5), () => {
                if (sheriam) {
                    sheriam.comm("emote produces the Great Serpent ring and places it in " + accepted.name + "'s hand.");
                    Util.returnRing(accepted);
                }
            }],
            [waitPulse(4), () => {
                accepted.send("The ring settles on your finger, warm and familiar. But something has changed. You have changed.");
            }],
            [waitPulse(6), () => {
                if (sheriam && summoner) {
                    sheriam.say(summoner.name + " Sedai, you may ask " + accepted.name + " which Ajah she wishes to join.");
                }
            }],
            [waitPulse(3), () => {
                if (summoner) {
                    summoner.send("Ask " + accepted.name + " which Ajah she chooses. She must say the name aloud.");
                }
            }]
        ]);
    };

    // =========================================================================
    // Ajah Selection
    // Script 22842 - Speech trigger on Sheriam
    // Listens for the Accepted to say an Ajah name
    // =========================================================================

    self.ajahChosen = function(self, actor, here, args) {
        var Util = AesSedaiRank.Util;

        if (Util.testPhase !== "ajahSelect") return;
        if (actor !== Util.accepted) return;

        let vArgs = getArgList(args.toLowerCase());
        let chosenColor = null;

        for (let i = 0; i < vArgs.length; i++) {
            if (Util.ajahMap[vArgs[i]] !== undefined) {
                chosenColor = vArgs[i];
                break;
            }
        }

        if (!chosenColor) {
            self.say("I did not understand. Which Ajah will you serve? Red, Blue, Green, Yellow, Brown, White, or Gray?");
            return;
        }

        Util.testPhase = "raising";
        Util.processRaising(chosenColor);
    };

    // =========================================================================
    // Process Raising (promotion, shawl, announcement)
    // Called after the Accepted names her Ajah
    // =========================================================================

    self.processRaising = function(ajahColor) {
        var Util = AesSedaiRank.Util;
        let accepted = Util.accepted;
        let sheriam = Util.sheriam;
        let ajahClanNum = Util.ajahMap[ajahColor];
        let ajahMobVnum = Util.mobIndex[ajahClanNum];
        let chamber = getRoom(Util.testingChamberVnum);
        let colorCap = capFirstLetter(ajahColor);
        const aesSedaiRank = 5;

        runSequence([
            [waitPulse(3), function() {
                if (sheriam) {
                    sheriam.say("So be it. " + accepted.name + " has chosen the " + colorCap + " Ajah.");
                }
            }],
            [waitPulse(5), () => {
                chamber.echo("The " + colorCap + " Ajah sister steps forward, bearing a shawl in the color of her Ajah.");
            }],
            [waitPulse(4), () => {
                accepted.send("She places the shawl about your shoulders with solemn reverence.");
                act("She places the shawl about $n's shoulders with solemn reverence.", false, accepted, null, null, constants.TO_ROOM);
                // Load and give the shawl
                let shawl = accepted.loadObj(20115);
                if (hasClanEquivalent(ajahClanNum, 20115)) {
                    shawl.setRetoolSDesc(getClanGearSDesc(ajahClanNum, 20115));
                    shawl.setRetoolName(getClanGearName(ajahClanNum, 20115));
                    shawl.setRetoolDesc(getClanGearDesc(ajahClanNum, 20115));
                }
            }],
            [waitPulse(5), () => {
                if (sheriam) {
                    sheriam.say("Welcome, Sister. Welcome to the " + colorCap + " Ajah, and welcome to the Aes Sedai.");
                }
            }],
            [waitPulse(3), () => {
                // Rank up in White Tower to Aes Sedai (rank 5)
                if (sheriam) {
                    if (accepted.getRank(constants.CLAN_WHITE_TOWER) < aesSedaiRank)
                        sheriam.comm("rank " + accepted.name + " " + constants.CLAN_WHITE_TOWER);
                }
            }],
            [waitPulse(2), () => {
                if (sheriam) {
                    if (accepted.getRank(constants.CLAN_WHITE_TOWER) < aesSedaiRank)
                        sheriam.comm("rank " + accepted.name + " " + constants.CLAN_WHITE_TOWER);
                }
            }],
            [waitPulse(2), () => {
                // Join the Ajah clan
                if (sheriam) {
                    sheriam.comm("clan " + accepted.name + " " + ajahClanNum);
                }
            }],
            [waitPulse(3), () => {
                //Only announce a raising that actually happened. The rank command can refuse for
                //reasons this script cannot see, and a false Tower-wide announcement is worse
                //than no announcement.
                if (accepted.getRank(constants.CLAN_WHITE_TOWER) < aesSedaiRank) {
                    mudLog(constants.BRF, 102, "Aes Sedai raising: " + accepted.name + " passed the test but could not be raised in the White Tower.");

                    if (sheriam)
                        sheriam.say("Something is amiss, " + accepted.name + ". Your raising cannot be entered into the books today. This will be looked into.");

                    return;
                }

                gecho("** " + accepted.name + " has been raised to the shawl of an Aes Sedai!");
            }],
            [waitPulse(2), () => {
                if (sheriam && accepted.getRank(constants.CLAN_WHITE_TOWER) >= aesSedaiRank) {
                    sheriam.comm("shout " + accepted.name + " has been raised to Aes Sedai of the " + colorCap + " Ajah!");
                }
            }],
            [waitPulse(8), () => {
                if (sheriam) {
                    sheriam.say("Go now, Sister, and serve the Light.");
                }
            }],
            [waitPulse(10), () => {
                // Cleanup and send everyone back to Sheriam's study
                Util.cleanupTestRooms();
                Util.cleanupTestingChamber();
                Util.sendEveryoneHome();
                Util.resetState();
            }]
        ]);
    };

    return self;
})();
