/**
 * Schedules a sequence of delayed actions, one at a time.
 * Each entry is [delayInPulses, callback].
 * The next step is only scheduled after the current one fires.
 * If a callback returns false, the sequence is aborted.
 */
function runSequence(steps) {
    let index = 0;
    function next() {
        if (index >= steps.length) return;
        let step = steps[index++];
        setTimeout(step[0], function() {
            if (step[1]() === false) return;
            next();
        });
    }
    next();
}