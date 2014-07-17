<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
User user = (User)request.getAttribute("User");
String sessionId = (String)request.getAttribute("SessionId");
%>

<script type="text/javascript">

var socket = null;
var commandProcessors = {};

commandProcessors["Load Item Flags"] = function(command)
{

	var $itemWearInputs = $("#itemWearPanel .inputs");
	var $itemTypeInputs = $("#itemTypePanel .inputs");
	var $itemExtraInputs = $("#itemExtrasPanel .inputs");

	$itemWearInputs.html("");

	command.itemTypes.forEach(function(itemType, index) {

		var $input = $("<input type='checkbox' name='ItemType'></input>");
		$input.val(itemType.id);
		$input.attr("id", "ItemType" + itemType.id);

		var $label = $("<label></label>");
		$label.text(itemType.name);
		$label.attr("for", "ItemType" + itemType.id);

		$itemTypeInputs.append($input).append($label);

		if((index + 1) % 3 == 0) {

			$itemTypeInputs.append("<br>");
		}

		socket.itemTypesMap[ parseInt(itemType.id) ] = itemType.name;
	});

	command.itemWears.forEach(function(itemWear, index) {

		var $input = $("<input type='checkbox' name='ItemWear'></input>");
		$input.val(itemWear.id);
		$input.attr("id", "ItemWear" + itemWear.id);

		var $label = $("<label></label>");
		$label.text(itemWear.name);
		$label.attr("for", "ItemWear" + itemWear.id);

		$itemWearInputs.append($input).append($label);

		if((index + 1) % 3 == 0) {

			$itemWearInputs.append("<br>");
		}

		socket.wearTypesMap[ parseInt(itemWear.id) ] = itemWear.name;
	});

	command.extraFlags.forEach(function(extraFlag, index) {

		var $input = $("<input type='checkbox' name='ItemExtra'></input>");
		$input.val(extraFlag.id);
		$input.attr("id", "ItemExtra" + extraFlag.id);

		var $label = $("<label></label>");
		$label.text(extraFlag.name);
		$label.attr("for", "ItemExtra" + extraFlag.id);

		$itemExtraInputs.append($input).append($label);

		if((index + 1) % 3 == 0) {

			$itemExtraInputs.append("<br>");
		}

		socket.extraFlagsMap[ parseInt(extraFlag.id) ] = extraFlag.name;
	});
};

commandProcessors["Load Object List"] = function(command)
{
	console.log("Objects: " + command.objects.length);
	var $tbody = $("#resultsTable > tbody");

	$tbody.children("tr:not(:first)").remove();
	command.objects.forEach(function(object) {


		var $row = $(	"<tr>"
					+	"<td class='objectIdRow'></td>"
					+	"<td class='shortDescriptorRow'></td>"
					+	"<td class='itemTypeRow'></td>"
					+	"<td class='wearFlagsRow'></td>"
					+	"<td class='extraFlagsRow'></td>"
					+	"<td class='weightRow'></td>"
					+	"<td class='costRow'></td>"
					+	"<td class='obRow'></td>"
					+	"<td class='dbRow'></td>"
					+	"<td class='pbRow'></td>"
					+	"<td class='absRow'></td>"
					+	"<td class='bashRow'></td>"
					+	"<td class='numRow'></td>"
					+	"</tr>"
					);

		var $tdArray = $row.children();

		$tdArray.eq(0).text(object.vnum);
		$tdArray.eq(1).text(object.shortDescription);
		$tdArray.eq(2).text(socket.itemTypesMap[object.itemType]);

		var wear = [];
		for(var wearTypeId in socket.wearTypesMap) {
			if(object.wearFlags & (1 << wearTypeId))
				wear.push(socket.wearTypesMap[ wearTypeId ]);
		}
		$tdArray.eq(3).text(wear.join(", "));

		var extra = [];
		for(var extraFlagId in socket.extraFlagsMap) {

			if(object.extraFlags & (1 << extraFlagId))
				extra.push(socket.extraFlagsMap[extraFlagId]);
		};
		$tdArray.eq( 4).text(extra.join(", "));
		$tdArray.eq( 5).text(object.weight.toFixed(2));
		$tdArray.eq( 6).text(object.cost);
		$tdArray.eq( 7).text(object.ob);
		$tdArray.eq( 8).text(object.db);
		$tdArray.eq( 9).text(object.pb);
		$tdArray.eq(10).text(object.abs);
		$tdArray.eq(11).text(object.bash.toFixed(4));
		$tdArray.eq(12).text(object.num);

		$tbody.append($row);
	});
}

commandProcessors["SessionID"] = function(command)
{
	socket.sendCommand( {method: "Load Item Flags"} );
}

$(document).ready(function() {
	socket = new WebSocket("ws://kinslayermud.org:<%=webSupport.getPlayerPortalServerPort()%>", "player-portal-protocol");

	socket.itemTypesMap = {};
	socket.wearTypesMap = {};
	socket.extraFlagsMap = {};

	var reduceInputs = function(elements) {

		return elements.map(function(element) {
			return element.value;
		});
	};

	socket.onopen = function()
	{
		this.sendCommand({
			method: "SessionID",
			userId: <%= user.getUserId() %>,
			sessionId: "<%= sessionId %>"
		});
	}
	socket.onclose = function()
	{
	}
	socket.onmessage = function(msg)
	{
		console.log("Received Response.");
		var command = JSON.parse(msg.data);
		
		var commandProcessor = commandProcessors[command.method];

		console.log("Response: " + command.method);
		if(commandProcessor)
		{
			commandProcessor(command);
		}
	}

	socket.sendCommand = function(command)
	{
		this.send(JSON.stringify(command) + String.fromCharCode(0x06));
	}

	$("#searchForm").on("submit", function(e) {

		e.preventDefault();

		var triggerValue = $("#TriggersInput").val().trim();

		var command = {
			method: "Load Object List",
			itemType: reduceInputs($("#itemTypePanel input:checked").toArray()),
			wearType: reduceInputs($("#itemWearPanel input:checked").toArray()),
			itemExtra: reduceInputs($("#itemExtrasPanel input:checked").toArray()),
			triggers: triggerValue.length == 0 ? [] : triggerValue.split(/[ ,]/),
			namelist: $("#NamelistInput").val(),
			shortDescription: $("#ShortDescriptionInput").val(),
			longDescription: $("#LongDescriptionInput").val()
		};

		var lowVnum = parseInt($("#lowVnumInput").val());
		var highVnum = parseInt($("#highVnumInput").val());
		var val0 = parseInt($("#Val1").val());
		var val1 = parseInt($("#Val2").val());
		var val2 = parseInt($("#Val3").val());
		var val3 = parseInt($("#Val4").val());
		var val4 = parseInt($("#Val5").val());
		var val5 = parseInt($("#Val6").val());
		var val6 = parseInt($("#Val7").val());
		var val7 = parseInt($("#Val8").val());
		var val8 = parseInt($("#Val9").val());
		var val9 = parseInt($("#Val10").val());
		var val10 = parseInt($("#Val11").val());
		//var val11 = parseInt($("#Val12").val());

		if(!isNaN(lowVnum))
			command["lowVnum"] = lowVnum;
		if(!isNaN(highVnum))
			command["highVnum"] = highVnum;
		if(!isNaN(val0))
			command["val0"] = val0;
		if(!isNaN(val1))
			command["val1"] = val1;
		if(!isNaN(val2))
			command["val2"] = val2;
		if(!isNaN(val3))
			command["val3"] = val3;
		if(!isNaN(val4))
			command["val4"] = val4;
		if(!isNaN(val5))
			command["val5"] = val5;
		if(!isNaN(val6))
			command["val6"] = val6;
		if(!isNaN(val7))
			command["val7"] = val7;
		if(!isNaN(val8))
			command["val8"] = val8;
		if(!isNaN(val9))
			command["val9"] = val9;
		if(!isNaN(val10))
			command["val10"] = val10;
//		if(!isNaN(val11))
//			command["val11"] = val11;

		socket.sendCommand(command);
	});

	$("#tableRowHeader > th > a").on("click", function(e) {

		e.preventDefault();
		var $this = $(this);
		var $th = $this.parent();
		var index = $.makeArray($th.parent().children()).indexOf($th[0]);
		var $tbody = $("#resultsTable > tbody");
		var $tableRows = $tbody.children("tr:not(:first-child)");

		var ascending = $this.data("sort") != "a";
		$this.data("sort", ascending ? "a" : "d");

		$tableRows.remove();

		var $tableRowsArray = $.makeArray( $tableRows );

		$tableRowsArray.sort(function(a, b) {

			var aText = $(a).children().eq(index).text(), bText = $(b).children().eq(index).text();
			var aNumber = Number(aText), bNumber = Number(bText);
			var returnValue;

			if( !isNaN(aNumber) && !isNaN(bNumber) )
			{
				if(aNumber < bNumber)
					returnValue = -1;
				else if(aNumber > bNumber)
					returnValue = 1;
				else
					returnValue = 0;
			}
			else
			{
				if(aText < bText)
					returnValue = -1;
				else if(aText > bText)
					returnValue = 1;
				else
					returnValue = 0;
			}

			return ascending ? returnValue : -returnValue;
		});

		$tableRowsArray.forEach(function($tableRow) {

			$tbody.append($tableRow);
		});
	});
});

</script>

<div id="topPanel">

	<form id="searchForm">

		Low Vnum: <input type="text" id="lowVnumInput" />
		High Vnum: <input type="text" id="highVnumInput" />

		Triggers: <input type="text" placeholder="space or comma separated" id="TriggersInput" class="input200" />
		
		<br/>
		<br/>
		
		Namelist: <input type="text" placeholder="regular expression" id="NamelistInput" class="input200" />
		Short Description: <input type="text" placeholder="regular expression" id="ShortDescriptionInput" class="input200" />
		Long Description: <input type="text" placeholder="regular expression" id="LongDescriptionInput" class="input200" />
		
		<br/>
		<br/>
		
		Val 1: <input type="text" id="Val1" /> Val 2: <input type="text" id="Val2" /> Val 3: <input type="text" id="Val3" />
		Val 4: <input type="text" id="Val4" /> Val 5: <input type="text" id="Val5" /> Val 6: <input type="text" id="Val6" />
		<br/>
		<br/>
		Val 7: <input type="text" id="Val7" /> Val 8: <input type="text" id="Val8" /> Val 9: <input type="text" id="Val9" />
		Val 10: <input type="text" id="Val10" /> Val 11: <input type="text" id="Val11" /> Val 12: <input type="text" id="Val12" />
		<br/>
		<br/>
		
		<div id="itemExtrasPanel">
			<div class="textCenter itemExtrasHeader">Item Extras</div>
			<div class="inputs"></div>
		</div>
		
		
		<div id="itemTypePanel">
			<div class="textCenter itemTypeHeader">Item Types</div>
            <div class="inputs"></div>
		</div>
		
		<div id="itemWearPanel">
			<div class="textCenter itemWearHeader">Item Wears</div>
            <div class="inputs"></div>
		</div>

		<br/>
		<br/>
		<button type="submit" class="flatGreyButton short" id="searchButton">Search</button>
	</form>
</div>

<div id="divider"></div>

<div id="seachResults">

	<table id="resultsTable" cellspacing="0">
		<tr id="tableRowHeader">
			<th><a href="#">VNum</a></ht>
			<th><a href="#">Short Description</a></th>
			<th><a href="#">Item Type</a></th>
			<th><a href="#">Wear Flags</a></th>
			<th><a href="#">Extra Flags</a></th>
			<th><a href="#">Weight</a></th>
			<th><a href="#">Cost</a></th>
			<th><a href="#">OB</a></th>
			<th><a href="#">DB</a></th>
			<th><a href="#">PB</a></th>
			<th><a href="#">ABS</a></th>
			<th><a href="#">Bash</a></th>
			<th><a href="#">Num</a></th>
		</tr>
	</table>

</div>

