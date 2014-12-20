<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>

<style type="text/css">

#racePanel
{
	width: 60%;
	margin: auto;
}

#raceSelection
{
	text-align: center;
}

#raceSelection div
{
	width: 50%;
	float: left;
	text-align: center;
}

#raceSelection div a,
.classSelection div a
{
	color: #FFF;
	text-decoration: none;
	font-size: 24px;
}

#raceSelection div a:hover,
.classSelection div a:hover
{
	color: #D10020;
}

#raceSelection div a.selected,
.classSelection div a.selected
{
	color: #D10020 !important;
	text-decoration: underline !important;
}

.hidden
{
	display: none;
}

.raceDescription
{
}


.classPanel
{
	width: 80%;
	margin: auto;
	margin-top: 1%;
}

.white
{
	color: #FFF !important;
}

.classPanel .header
{
	text-align: center;
	font-size: 22px;
	font-weight: bold;
}

.classSelection
{
}

.classSelection div
{
	float:left;
	width: 25%;
	text-align: center;
}

.classDescription
{
	width: 74.5%;
	margin: auto;
}
</style>

<script type="text/javascript">

$(document).ready(function() {

	$("#selectTrollocAnchor").bind("click", function(event) {

		$(".classPanel").addClass("hidden");
		$(".raceDescription").addClass("hidden");
		$("#trollocDescription").removeClass("hidden");
		$("#trollocClassPanel").removeClass("hidden");

		$("#raceSelection div a").removeClass("selected");
		$(this).addClass("selected");
		event.preventDefault();
	});

	$("#selectHumanAnchor").bind("click", function(event) {

		$(".classPanel").addClass("hidden");
		$(".raceDescription").addClass("hidden");
		$("#humanDescription").removeClass("hidden");
		$("#humanClassPanel").removeClass("hidden");

		$("#raceSelection div a").removeClass("selected");
		$(this).addClass("selected");
		event.preventDefault();
	});

	$(".classSelection div a").bind("click", function(event) {
		event.preventDefault();

		$(".classSelection div a").removeClass("selected");
		$(this).addClass("selected");

		var selectedRace = $(this).closest(".classPanel").attr("race");
		var selectedClass = $(this).text().toLowerCase();
		var descriptionElement = $("#" + selectedRace + selectedClass + "description");

		$(".classDescription").addClass("hidden");
		descriptionElement.removeClass("hidden");
	});
});

</script>

		<div id="pageContent" class="noTopMargin autoWidth">

			<div id="pageTitle" class="white">Select Your Race:</div>

			<!-- RACES -->
			<div id="racePanel">

				<div id="raceSelection">
					<div><a href="#" id="selectTrollocAnchor">Trolloc</a></div>
					<div><a href="#" id="selectHumanAnchor">Human</a></div>
				</div>
				<div class="clearLeft"></div>

				<div id="humanDescription" class="hidden raceDescription">The Human race consists of the main forces of the Light. They have certain perks, primarily the vast, less hostile land in their control. Humans are also able to move around on horseback, giving them more flexibility in their movement. Along with the greater land quantity comes a larger number of cities and clans, and the politics that come with it.
				</div>
				<div id="trollocDescription" class="hidden raceDescription">A Trolloc is a type of Shadowspawn created in the Age of Legends that comprises the bulk of the Dark One's armies. A crossing of human and animal stock, their physical characteristics are a blend of both lineages, horrifying almost all who come in contact with them. While capable of speech, they possess and intellect inferior to that of other sentient races and have proven difficult to utilize in battle unless supervised. They are carnivorous, sadistic, fearful and violent. Unlike other Shadowspawn, Trollocs are social creatures, grouped by several distinct tribes and capable of cruel and twisted humor.
				</div>
			</div>




			<!-- CLASSES -->
			<div id="trollocClassPanel" class="hidden classPanel" race="trolloc">
				<div class="header white">Select Your Class:</div>
				<div class="classSelection">
					<div><a href="#">Warrior</a></div>
					<div><a href="#">Ranger</a></div>
					<div><a href="#">Thief</a></div>
					<div><a href="#">Dreadguard</a></div>
				</div>
				<div id="trollocwarriordescription" class="classDescription hidden">
					Trolloc Warriors have a slight edge in their strength capacity. As with the human warrior race, all warrior skills cost a single practice session to improve. Trolloc Warriors are able to roll as high as a 21 strength status point.
				</div>

				<div id="trollocrangerdescription" class="classDescription hidden">
					Trolloc Rangers have a slight edge in their constitution capacity, able to roll as high as a 20 status point. As with human rangers, trolloc rangers are able to practice ranger skills at a cost of just one practice session. Rangers are also able to practice both warrior and thief skills at a cost of two, making them the most versatile class in the game.
				</div>

				<div id="trollocthiefdescription" class="classDescription hidden">
					Trolloc Thieves are able to roll as high as a 20 dexterity status point, and are able to practice Thief skills at a cost of just one practice session. Warrior skills are expensive for Thieves, costing 3 sessions, while Ranger skills cost 2. For this reason, Thieves strive at surreptitious methods of attack, including sneaking, hiding, and backstabbing their opponents.
				</div>

				<div id="trollocdreadguarddescription" class="classDescription hidden">
					Dreadguards are the channeling class of the Darkside. They are humans who have changed allegiance to the Shadow. They are able to ride horses and speak both the Trolloc and Common tongue. Their skill in the art of the One Power makes them an essential asset to the Shadow. Dreadguards who reach high rank are eventually able to advance to the status of Dreadlord.
				</div>
			</div>
			<div id="humanClassPanel" class="hidden classPanel" race="human">
				<div class="header white">Select Your Class:</div>
				<div class="classSelection">
					<div><a href="#">Warrior</a></div>
					<div><a href="#">Ranger</a></div>
					<div><a href="#">Thief</a></div>
					<div><a href="#">Channeler</a></div>
				</div>
				<div id="humanwarriordescription" class="classDescription">
					Warriors strive at melee combat, able to practice weapon skills, riding, and physical attacks such as charge and bash at a low cost. Warriors are great for dealing out a great deal of damage, but lack in skills that allow them to avoid being hit, making heavy armor an appealing setup for them.
				</div>

				<div id="humanrangerdescription" class="classDescription">
					Rangers are a versatile class, as they can train in the skills of Thieves and Warriors for a mid-level cost. Ranger skills are also essential for all classes to train in, as they unlock some of the key abilities required to survive and endure in the game.
				</div>

				<div id="humanthiefdescription" class="classDescription">
					Thieves are the sneaky class of the game, excelling in the art of surreptitious combat. Included are skills such as sneaking, the act of moving between rooms unseen and backstabbing, a quick, brutal attack that the target doesn't see coming. Thieves are more dominant in lighter gear and primarily use shorter blades.
				</div>

				<div id="humanchannelerdescription" class="classDescription">
					Channelers are a powerful class, as they are able to wield the One Power and cast weaves against their opponents or to their own benefit. Although they possess this massive advantage, training in physical skills is much more difficult for channelers, as skills in each of the other three classes cost a great deal more for them.
				</div>
			</div>

		</div>