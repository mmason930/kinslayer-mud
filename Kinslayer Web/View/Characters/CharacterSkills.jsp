<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
	<tiles:insertTemplate template="/View/Characters/Framework/SubNav.jsp" flush="true" />
	<div class="container_box">
	
		<div style="margin: 20px; float:left;">
		
			<div>
			
			<h1>
				Character Skills
			</h1>
			
			<p>	In order for a character to acquire skills he or she must find instructors that are willing to teach ranger, thief, warrior, or channelling skills.
				In the KinslayerMUD world this is known as <span class="bold">"practicing"</span>.
				When you find an instructor, type <span class="bold">"practice"</span> in his room and you will be presented with a list of the skills that can be practiced along with how many practice sessions remaining, and the cost of practicing a skill.
				If a skill interests you, type <span class="bold">"practice [skill_name]"</span> to acquire that skill.
				For example <span class="bold">"practice shield parry"</span> to learn acquire the <span class="bold">"shield parry"</span> skill.
				This will return a message of a percentage practiced in that skill.
				This is normal; one must practice a skill several times to master it.
				A skill level of 90% or more is recommended before being dependable.
			</p>

			<p>	Characters can practice their class\'s skills for the cost of 1 session each.
				Other class's skills come at a cost of 2 practice sessions each.
				For example if your character is a thief, he will spend only 1 practice session for the "backstab" skill which is a thief skill, but will have to spend 2 practice sessions for the "Shield Parry" warrior skill.
			</p>
			
			<p>Instructors can be found all over the world, however here are a few that can easily be found:</p> <br />

			<h2>Human</h2>
			<p><span class="bold">In the city of Caemlyn:</span> <br />
				<span class="bold">Ranger</span> Practice Area: From the Caemlyn Central Square go west, north (w, n). <br /> <br />
				<span class="bold">Warrior</span> Practice Area: From the Caemlyn Central Square go west (w).   <br /> <br />
				<span class="bold">Thief</span> Practice Area: From the Caemlyn Central Square go west, west (2w). <br />  <br />
				<span class="bold">Male Channeler</span> Practice Area: A man named "Logain" can be found west of Fal Dara.  <br /><br />
				<span class="bold">Female Channeler</span> Practice Area: Sheriam Sedai can be found in the White Tower.   <br /> <br />
				<span class="bold">In the area South of Lugard: </span> <br />
				<span class="bold">Warrior</span> Practice Area: Located right at the starting point, at a man called "Faywin". <br /> <br />
				<span class="bold">Ranger</span> Practice Area: From Faywin, north, west, south (n, w, s) at a man called "Vitan". <br /><br />
				<span class="bold">Thief</span> Practice Area: From Faywin, north, west, south, south, east (n,w,s,s,e) at a woman called "Rensha".
			</p> <br />

			<h2>Darkside</h2>
			<p><span class="bold">In the Trolloc Keep:</span><br /> <br />
				<span class="bold">Warrior</span> Practice Area: From "Before a Large Tower" (the well), go east, east, east, north, north, west, west, north, north, north, west, west, west, up, east, east, east, north, north (3e, 2n, 2w, 3n, 3w, u, 3e, 2n).<br /><br />
				<span class="bold">Ranger</span> Practice Area: From "Before a Large Tower" (the well), go east, east, east, north, north, west, west, north, north, north, north, north, north, west (3e, 2n, 2w, 6n, w).  <br />   <br />
				<span class="bold">Thief</span> Practice Area: From "Before a Large Tower" (the well), go east, east, east, north, north, west, west, north, north, north, east, east, east, south, south, west (3e, 2n, 2w, 3n, 3e, 2s, w).  <br /><br />
				<span class="bold">Female Dreadguard</span> Practice Area: From "Before a Large Tower" (the well), go north, west (n,w). <br />   <br />
				<span class="bold">Male Dreadguard</span> Practice Area: From "Before a Large Tower" (the well), go north, west (n,w), same as above. <br />  <br />
				<span class="bold">*Note:</span> Trade practice sessions are discussed in the "Trades" section.
			</p>

			</div>
		</div>
		<div style="clear: both;"></div>
	</div> <!-- End of Container Box -->