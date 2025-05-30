let script13233 = function(self, actor, here, args, extra) {

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

    commandsToUse = commandsToUse[random(0, commandsToUse.length - 1)];
    self.comm(commandsToUse);
}