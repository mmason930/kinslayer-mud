var script201 = function(self, actor, here, args, extra)
{
	if( !str_cmp(args,"Norum, go gamble.") && !str_cmp(args,"Norum go gamble") && !str_cmp(args,"Norum, go gamble") && !str_cmp(args,"Norum go gamble.") )
    return;
waitpulse 5;
var famousPeople = ["Thomas Jefferson", "Benjamin Franklin", "George Washington", "Abraham Lincoln", "John Adams", "William Whipple", "William Williams", "Richard Henry Lee", "Philip Livingston", "Oliver Wolcott", "Matthew Thornton", "John Hart", "George Taylor", "Elbridge Gerry", "Carter Braxton", "Caesar Rodney"];
var description = ["My boy", "My man", "My homie", "My hero", "My idol", "The badass that is", "The great"];
var verbs = ["dictates", "forecasts", "foretells", "predicts", "forespeaks", "presumes"];
var outcome = ["demise", "loss", "defeat", "annihilation", "death by catooga", "slaughter", "extermination", "failure", "blunder", "massacre", "fatality", "perdition", "undoing"];
var randF = random(0,famousPeople.length-1);
var randD = random(0,description.length-1);
var randV = random(0,verbs.length-1);
var randO = random(0,outcome.length-1);
self.say(description[randD] + " " + famousPeople[randF] + " " + verbs[randV] + " your " + outcome[randO] + ".");
//Now, Norum will make his wikipedia comment.
waitpulse 5;
var subjects = ["Galnor", "Tulon", "Kison", "Avaine", "Fogel", "Rhollor", "LOLOL Narg", "Gristlyn", "Myrabitch", "Smasher", "Razina", "TrinOMG", "Chasey"];
var randS = random(0,subjects.length-1);
self.say("Don't tell " + subjects[randS] + ", but I just looked that up on wikipedia.");
//Norum rolls
waitpulse 5;
var dice1 = random(1,6);
var dice2 = random(1,6);
var sTotal = dice1+dice2;
here.echoaround(self,self.name + " tosses the dice. They land on " + dice1 + " and " + dice2  + ".");
self.send("You toss the dice. They land on " + dice1 + " and " + dice2 + ".");
//User rolls
waitpulse 5;
var dice1 = random(1,6);
var dice2 = random(1,6);
var aTotal = dice1+dice2;
here.echoaround(actor,actor.name + " tosses the dice. They land on " + dice1 + " and " + dice2  + ".");
actor.send("You toss the dice. They land on " + dice1 + " and " + dice2 + ".");
if( sTotal == aTotal )
{//TIE
    here.echo("TIE");
}
else if( sTotal > aTotal )
{//Norum wins
    here.echo("Norum wins");
}
else if( aTotal > sTotal )
{
    here.echo(actor.name + " wins.");
}

};

