let script13233 = function(self, actor, here, args, extra) {

    if(self.fighting) {

        const channelingRoll = random(1, 100);

        if(channelingRoll <= 25) {
            act("$n extends $s arms towards the ground, which begins shaking violently!", false, self, null, null, constants.TO_ROOM);
            act("You extend your arms towards the ground, which begins shaking violently!", false, self, null, null, constants.TO_CHAR);
            here.people.forEach(function (personInRoom) {
                if (personInRoom.isValid && personInRoom.fighting === self) {
                    personInRoom.damage(random(15, 40), self);
                }
            });
        } else if(channelingRoll <= 50) {
            act("$n extends his arms outwards towards $N, and a fireball fires from $s hands!", false, self, null, self.fighting, constants.TO_NOTVICT);
            act("$n extends his arms outwards towards you, and a fireball fires from $s hands!", false, self, null, self.fighting, constants.TO_VICT);
            act("You extend your arms outwards towards $N and a fireball fires from your hands!", false, self, null, self.fighting, constants.TO_CHAR);
            self.fighting.damage(random(35, 75), self);
        }

        return;
    }
    
    const seenPlayers = here.people.filter(function(personInRoom) {
        if(!personInRoom.isMob() && self.canSee(personInRoom)) {
            return true;
        }
    });

    const sectionRoll = random(1, 100);
    let commandsToUse = null;

    if(sectionRoll < 80) {
        commandsToUse = [
            "yawn",
            "think",
            "whistle",
            "daydream",
            "think",
            "grunt",
            "grumble",
            "cough"
        ];
    } else if(sectionRoll < 90) {
        commandsToUse = [
            "hop",
            "hiss",
            "laugh",
            "drool",
            "dance",
            "curse",
            "worship",
            "trot"
        ];
    } else if(sectionRoll < 95 && seenPlayers.length === 0) {
        commandsToUse = [
            "emote extends his hands outwards and a violent gust of air appears in front of him.",
            "emote raises his arms the clouds above begin consolidating.",
            "emote looks down and the ground begins shaking violently."
        ]
    }

    if(commandsToUse !== null) {
        commandsToUse = commandsToUse[random(0, commandsToUse.length - 1)];
        self.comm(commandsToUse);
    }
}
