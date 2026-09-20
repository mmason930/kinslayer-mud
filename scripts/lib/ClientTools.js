// The companion observes script 501; it never advances a lesson or grants rewards.
global.clientTools = {
    tutorial: function(actor) {
        if (!actor.countJS(501)) return {active: false};
        var step = Number(getSval(actor, 501, "tutorial")) || 0;
        var weapon = actor.class === constants.CLASS_CHANNELER ? "spear" :
            actor.class === constants.CLASS_THIEF ? "knife" : actor.class === constants.CLASS_RANGER ? "sword" : "mace";
        var lessons = [
            ["Listen to the Guardian", "The Guardian is speaking. Follow the lesson in the game output.", "", "tutorial"],
            ["Meet the Guardian of Life", "The Guardian is offering the starter tutorial. Answer yes to begin, or no to decline.", "yes", "tutorial"],
            ["Look around", "Use LOOK to see the room, its description, people and exits.", "look", "look"],
            ["Check your score", "Use SCORE to learn about your character and needs.", "score", "score"],
            ["Drink from the fountain", "The Guardian has made you thirsty. Drink from the fountain here.", "drink fountain", "drink"],
            ["Check your inventory", "The Guardian gave you a flask. Use INVENTORY to continue the lesson.", "inventory", "inventory"],
            ["Ask for your starting kit", "Ask the Guardian for the equipment used in this tutorial.", "tell guardian kit", "tell"],
            ["Wear your equipment", "Equip the kit the Guardian gave you.", "wear all", "wear"],
            ["Remove your dagger", "Make room for the new weapon by removing your dagger.", "remove dagger", "remove"],
            ["Wield your new weapon", "The Guardian chose a " + weapon + " for your class.", "wield " + weapon, "wield"],
            ["Check your equipment", "Use EQUIPMENT to check what you are wearing and continue the lesson.", "equipment", "equipment"],
            ["Travel east", "Follow the Guardian's instruction to head east to the training area.", "east", "movement"],
            ["Light the darkness", "Hold one of the torches from your kit so you can see.", "hold torch", "hold"],
            ["Practice combat", "Look for the brown deer and attack it when you are ready.", "kill deer", "kill"],
            ["Finish the fight", "Watch the combat in the game output. The next lesson begins when the deer is defeated.", "", "combat"],
            ["Eat some food", "Eat the meat from your starting kit as the Guardian instructs.", "eat meat", "eat"],
            ["Return to the Guardian", "Head west for the final lesson.", "west", "movement"],
            ["Say hello", "Say hello to finish the communication lesson. Then listen to the Guardian's parting advice.", "say hello", "say"]
        ];
        var lesson = lessons[step] || lessons[0];
        return {active: true, step: step, total: 17, title: lesson[0], text: lesson[1], command: lesson[2], help: lesson[3]};
    },
    response: function(json, actor) {
        var request = JSON.parse(json), result = {};
        if (request.action === "settings") {
            if (!request.settings || typeof request.settings !== "object" || Array.isArray(request.settings)) throw new Error("Invalid preferences");
            var text = JSON.stringify(request.settings);
            if (text.length > 12000) throw new Error("Preferences too large");
            if (actor.setPval("WEB_CLIENT_SETTINGS", text, true) !== true)
                return JSON.stringify({saved: false, error: "Preferences could not be saved to your character. Your browser copy is retained; please try again."});
            return JSON.stringify({saved: true});
        }
        result.tutorial = this.tutorial(actor);
        if (request.action === "init") {
            try { result.settings = JSON.parse(actor.getPval("WEB_CLIENT_SETTINGS") || "null"); } catch (_) {}
            result.topics = JSON.parse(global.helpManager.getBrowserResponse(JSON.stringify({action: "index"}), actor)).topics || [];
            // Only enrich the already permission-filtered index. Descriptions may
            // contain executable help templates, so they are never sent here.
            result.topics.forEach(function(topic) {
                var file = global.helpManager.getHelpFileById(topic.id);
                topic.syntax = file && file.syntax || "";
            });
        }
        return JSON.stringify(result);
    }
};
