// Read-only web view of accepted quests. No client text is evaluated as code.
// Look quests up afresh so live quest edits and direct qval changes are reflected.
global.questBrowser = {
    getBrowserResponse: function(json, actor) {
        var command = JSON.parse(json);
        var response = {method: "Browse Quests", requestId: command.requestId};
        if (!actor || !actor.isValid || actor.vnum !== -1) {
            response.error = "Sign in as your character to view quests.";
        } else if (command.action !== "snapshot" || !Array.isArray(command.ids) || command.ids.length > 4 ||
            !command.ids.every(function(id) { return typeof id === "number" && isFinite(id) && id > 0 && id % 1 === 0; })) {
            response.error = "Invalid quest request.";
        } else {
            response.quests = [];
            Quest.allQuests.forEach(function(quest) {
                // Do not expose available/unaccepted quests, even if their ID is supplied.
                if (!quest.hasBegun(actor)) return;
                var item = {id: quest.id, name: quest.name};
                if (command.ids.indexOf(quest.id) !== -1) {
                    try {
                        item.tasks = [];
                        var allComplete = true;
                        quest.tasks.forEach(function(task) {
                            if (!task.hasUnlocked(actor)) { allComplete = false; return; }
                            var status = task.getStatus(actor), required = Number(task.completedValue);
                            var current = Number(status.progress);
                            if (!isFinite(current) || !isFinite(required) || required < 0) throw Error("Invalid task progress");
                            item.tasks.push({name: task.name, current: Math.max(0, current), required: required, completed: !!status.completed});
                            if (!status.completed) allComplete = false;
                        });
                        item.ready = allComplete;
                        if (command.detailId === quest.id) item.summary = global.questBrowser.summary(actor, quest);
                    } catch (error) {
                        // A broken quest must not hide other quests or repeatedly execute a bad summary.
                        item.error = "This quest's details are unavailable. Please report it to a staff member.";
                        delete item.tasks;
                    }
                }
                response.quests.push(item);
            });
        }
        return JSON.stringify(response);
    },
    summary: function(actor, quest) {
        // Match the journal's staff-authored dynamic summaries, with local colors.
        var nrm = "\x1b[0m", bld = "\x1b[1m", red = "\x1b[31m", grn = "\x1b[32m",
            yel = "\x1b[33m", blu = "\x1b[34m", mag = "\x1b[35m", cyn = "\x1b[36m", whi = "\x1b[37m";
        var summary = quest.summary || "";
        try { summary = eval(summary); } catch (error) { /* The journal also accepts plain text. */ }
        return String(summary == null ? "" : summary);
    }
};
