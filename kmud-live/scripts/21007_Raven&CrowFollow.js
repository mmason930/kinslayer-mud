var script21007 = function(self, actor, here, args, extra)
{
	for( var i = 0;i < here.people.length;++i )
{
    var person = here.people[i];
    if( (person.race == constants.RACE_HUMAN || person.race == constants.RACE_AIEL || person.race == constants.RACE_SEANCHAN) && person.vnum == -1 && (!self.leader || self.leader.room != self.room) )
    {
        self.comm("follow " + person.name);
        return;
    }
}

};

