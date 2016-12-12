var MercUtil = (function(MercUtil) {

	MercUtil.performCommands = function(actor, target, vArgs, here, myMercObj)
	{
		actor.send("ENABLED:");
		actor.send("Cmd Name             Arguments");
		actor.send("--------------------------------------------");
		actor.send("Assist          <Argument: NPC/PC to Assist>");
		actor.send("Bash            <No Argument>"); //Warrior + Ranger only
		actor.send("Crawl           <Argument: Entrance|Out>");
		actor.send("Dism            <No Argument>");
		actor.send("Dismount        <No Argument>");
		actor.send("Down            <No Argument>");
		actor.send("Drop            <Argument: itemName>");
		actor.send("East            <No Argument>");
		actor.send("Flee            <No Argument>");
		actor.send("Grab|Hold       <Argument: itemName>");
		actor.send("Hide            <No Argument>"); //Thief only
		actor.send("Inventory       <No Argument>");
		actor.send("Inv             <No Argument>");
		actor.send("North           <No Argument>");
		actor.send("Open            <Argument: doorName>"); //Ranger opens hidden only
		actor.send(cyn + "Opening hidden doors costs fourty (" + ylw + "40" + cyn + ") copper." + nrm);
		actor.send("Pick            <Argument: doorName>"); //Thief only
		actor.send(cyn + "Picking locked doors costs sixty (" + ylw + "60" + cyn + ") copper." + nrm);
		actor.send("Put             <Arguments: itemName containerName>");
		actor.send("Rendezvous      <Argument: None|Here>");
		actor.send("Rest            <No Argument>");
		actor.send("Save            <No Argument>");
		actor.send("Sleep           <No Argument>");
		actor.send("South           <No Argument>");
		actor.send("Stand           <No Argument>");
		actor.send("Rem             <Argument: itemName>");
		actor.send("Remove          <Argument: itemName>");
		actor.send("Report          <No Argument>");
		actor.send("Rescue          <Argument: NPC/PC Being Rescued>"); //
		actor.send("Return          <Argument: Home>");
		actor.send("Ride            <Argument: horseName>");
		actor.send("Sneak           <No Argument>"); //Thief only
		actor.send("Status          <No Argument>");
		actor.send("Take|Get        <Argument: itemName [containerName]>");
		actor.send("Up              <No Argument>");
		actor.send("Wake            <No Argument>");
		actor.send("West            <No Argument>");
		actor.send("Wield           <Argument: itemName>");
		actor.send(cyn + "Address all complaints, suggestions, or bugs to Rhollor on the forums(http://kinslayermud.org/forums) or in game." + nrm);
	};

	return MercUtil;
})(MercUtil || {});
