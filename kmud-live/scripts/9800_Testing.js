var script9800 = function(self, actor, here, args, extra)
{
	_block;
if (!actor.eq(constants.WEAR_WIELD) || actor.eq(constants.WEAR_WIELD).value(constants.VALUE_WEAPON_TYPE) != constants.WEAPON_SHORT_BLADE)
{
    actor.send("You would need a short blade to even attempt this.");
    return;
}
if (actor.position == 7)
{
    actor.send("No way! You're fighting for your life here!");
    return;
}
else if (actor.position < 7)
{
    actor.send("Perhaps you should get on your feet first.");
    return;
}
else
{
    here.echo("Moving on to variable setting.");
    var weap = actor.eq(constants.WEAR_WIELD);
    var target = actor.getCharRoomVis(args.split(" ")[1]);
    var mindmg = weap.value(constants.VALUE_WEAPON_BACKSTAB_LOW);
    var maxdmg = weap.value(constants.VALUE_WEAPON_BACKSTAB_HIGH);
    var dmg = random(mindmg, maxdmg);
    var landroll = random(1, 100);
    actor.startTimer(3);
    runTimer(actor)
    if (landroll < weap.value(constants.VALUE_WEAPON_BACKSTAB_RATING) + 1)
    {
        actor.send("You spring from your hiding spot and plunge " + weap.name + " into the back of " + target.name + ".");
        target.send("Someone stabs you you in the back. OUCH!");
        echoaroundmany([actor,target], actor.name + " bursts into view and thrusts " + weap.name + " into " + target.name + "'s back.");
        target.damage(dmg);
    }
    else
    {
        actor.send("You emerge from your hiding spot just as " + target.name + " turns around, ruining your attack.");
        here.echoaround(actor, actor.name + " steps into view at an inopportune moment, botching " + actor.hisher + " attack.");
    }
}

};

