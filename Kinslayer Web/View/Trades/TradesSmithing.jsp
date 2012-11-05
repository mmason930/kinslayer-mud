<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
	<tiles:insertTemplate template="/View/Trades/Framework/SubNav.jsp" flush="true" />
	<div class="container_box">
	
		<div style="margin: 20px; float:left;">
		
			<div>
			
			<h1>
				Introduction to Smithing
			</h1>
			
			<p>After mining for various metals, one can bring them to a furnace to refine the metal into bars. Both sides have their own smithing areas,
			click on the guides to the left to find out further information.</p>
		
			<p>The following is a list of items that can be smithed: </p>
		
			<p>
				<span class="bold">Tin</span> <br />
				<pre>
				WEAPONS                                 MATERIALS                  COST
				--------------------------------------------------------------------------
				a tin throwing knife              00 sm, 01 lg, 02 coal          28 copper
				a tin throwing spear              00 sm, 02 lg, 05 coal          88 copper
				</pre>
				<br />
				<span class="bold">Copper</span> <br />
				<pre>
				WEAPON                                  MATERIALS                  COST
				--------------------------------------------------------------------------
				a copper throwing knife           00 sm, 01 lg, 02 coal          44 copper
				a copper throwing spear           00 sm, 02 lg, 05 coal         120 copper
				</pre>
				
				 <br />
				<span class="bold">Aluminum</span>  <br />
				<pre>
				WEAPON                                  MATERIALS                  COST
				--------------------------------------------------------------------------
				an aluminum throwing knife        00 sm, 01 lg, 02 coal          60 copper
				an aluminum throwing spear        00 sm, 02 lg, 05 coal         136 copper
				</pre>
				 <br />
				<span class="bold">Aluminum</span><br />
				<pre>
				WEAPON                                  MATERIALS                  COST
				--------------------------------------------------------------------------
				iron throwing knife            00 sm, 01 lg, 02 coal         103 copper
				an iron throwing spear            00 sm, 02 lg, 05 coal         241 copper
				</pre>
				<br />
				<span class="bold">Steel</span> <br />
				<pre>
				WEAPON                                  MATERIALS                  COST
				--------------------------------------------------------------------------
				a steel throwing knife            00 sm, 01 lg, 02 coal         189 copper
				a steel throwing spear            00 sm, 02 lg, 05 coal         410 copper
				
				ARMOR                                   MATERIALS                  COST
				 --------------------------------------------------------------------------
				a pair of steel-plated war boots  04 sm, 05 lg, 05 coal         275 copper
				a pair of black steel war boots   05 sm, 06 lg, 07 coal         415 copper
				</pre>
				<br />
				<span class="bold">Silver</span>  <br />
				<pre>
				WEAPON                                  MATERIALS                  COST
				--------------------------------------------------------------------------
				a silver throwing knife           00 sm, 01 lg, 02 coal         423 copper
				a silver throwing spear           00 sm, 02 lg, 05 coal         928 copper
				</pre>
				<br />
				<span class="bold">Gold</span> <br />
				<pre>
				WEAPON                                  MATERIALS                  COST
				--------------------------------------------------------------------------
				a golden throwing knife           00 sm, 01 lg, 02 coal          11 silver
				a golden throwing spear           00 sm, 02 lg, 05 coal          26 silver
				</pre>
				 <br />
				<span class="bold">Platinum</span> <br />
				<pre>
				WEAPON                                  MATERIALS                  COST
				--------------------------------------------------------------------------
				a platinum throwing knife         00 sm, 01 lg, 02 coal          23 silver
				a platinum throwing spear         00 sm, 02 lg, 05 coal          61 silver
				</pre>
				<br />
				<span class="bold">Obsidian</span><br />
				<pre>
				WEAPON                                MATERIALS                    COST
				--------------------------------------------------------------------------
				an obsidian throwing knife        01 brick, 02 coal              82 silver
				an obsidian throwing spear        02 brick, 05 coal             149 silver
				ARMOR                                 MATERIALS                    COST
				--------------------------------------------------------------------------
				an obsidian ring, flecked with gold  05 brick, 10 coal          346 silver
				</pre>
			</p>
			
			<h2>Smithing as a Human</h2>
			
			<span class="bold">Required: a set of bellows</span>
			<p>Sets of bellows can be purchased from supply stores or other shops in human cities across the world. For example in Caemlyn at the "Caemlyn Supplies" shop (found north, west, west, north, and east (n, 2w, n, e) from the Caemlyn Plaza). To purchase a pick simply [type] "buy bellows".</p>
			<br />
			<span class="bold">Finding a Refinery</span>
			
			<p>There are a few options available to refine the ores you have mined. There is a furnace located just west of Whitebridge: From the western base of WhiteBridge head 10w, n, 4w, s, w, n, 7w, 2n, 3w, s,w. Another refinery can be found in Caemlyn, from Caemlyn's Eastern gates head n, n, n, e, n, n, w (3n e 2n w).</p>
			<br />
			<span class="bold">Refining Ore</span>
			
			<p>Once you have reached a furnace, ensure that you are wielding the bellows [type] "wield bellows". With the ore in your inventory, type "refine ore_type". Ore_type is the type of ore you have, such as tin, coal, copper etc.</p>
			<br />
			<span class="bold">What's next?</span>
			<p>After refining the ore you have mined into bars, you can use these bars to make weapons in a blacksmith's shop. You can find a blacksmith in Caemlyn, from the eastern gates go n,n,n,e,n,n,w (3n e 2n w). You can get information on what is required to make weapons by saying the type of metal. So for weapons made from tin, inquire about tin [type] "say list tin". If you want to know about weapons you can make with obsidian [type] "say list obsidian". The Blacksmith will provide you with more information if you [type] "say help".</p>

			
			<h2>Smithing on the Shadow</h2>
			
			<span class="bold">Required: a set of bellows</span>
			<p>Sets of bellows can be purchased from supply stores or other shops. For example in the Trolloc Keep at the "A Putrid Butcher shop" shop (found west, west, north, north, and west from the Keep Well in Front of the Large Tower). To purchase a bellows simply [type] "buy bellows".</p>
			<br />
			<span class="bold">Finding a Refinery</span>

			<p>The Dark Side furnace is located North and East of the Gate Keep. From the well "Before the Large Tower" head 3e 2n 2w 3n 3e open woodengate n, then go 2n e 3n 3e 3n e.</p>
			<br />
			<span class="bold">Refining Ore</span>

			<p>Once you have reached a furnace, ensure that you are wielding the bellows [type] "wield bellows". With the ore in your inventory, type "refine ore_type". Ore_type is the type of ore you have, such as tin, coal, copper etc.</p>
 			<br />
			<span class="bold">What's next?</span>
			<p>After refining the ore you have mined into bars, you can use these bars to make weapons in a blacksmith's shop. You can find a blacksmith in the trolloc keep at "An Unstable Smithy", 2w 4n from blood well in Gate Keep. You can get information on what is required to make weapons by saying the type of metal. So for weapons made from tin, inquire about tin [type] "say list tin". If you want to know about weapons you can make with obsidian [type] "say list obsidian". A list of what can be smithed is provided in the introduction. The Blacksmith will provide you with more information by [type] "say help".</p>


			</div>
		</div>
		<div style="clear: both;"></div>
	</div> <!-- End of Container Box -->