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
	var $itemTypeSelect = $("#itemTypeSelect");
	command.itemTypes.forEach(function(itemType) {

		var $option = $("<option></option>");
		$option.attr("value", itemType.id);
		$option.text(itemType.name);
		$itemTypeSelect.append($option);

		socket.itemTypesMap[ parseInt(itemType.id) ] = itemType.name;
	});

	var $itemWearTypeSelect = $("#itemWearTypeSelect");
	command.itemWears.forEach(function(itemWear) {

		var $option = $("<option></option>");
		$option.attr("value", itemWear.id);
		$option.text(itemWear.name);
		$itemWearTypeSelect.append($option);

		socket.wearTypesMap[ parseInt(itemWear.id) ] = itemWear.name;
	});

	command.extraFlags.forEach(function(extraFlag) {

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

		var command = {
			method: "Load Object List",
			itemType: parseInt($("#itemTypeSelect").val()),
			wearType: parseInt($("#itemWearTypeSelect").val())
		};

		var lowVnum = parseInt($("#lowVnumInput").val());
		var highVnum = parseInt($("#highVnumInput").val());

		if(!isNaN(lowVnum))
			command["lowVnum"] = lowVnum;
		if(!isNaN(highVnum))
			command["highVnum"] = highVnum;

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
		<select id="itemTypeSelect">
			<option>All Item Types</option>
		</select>

		<select id="itemWearTypeSelect">
			<option>All Item Wears</option>
		</select>

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
		</tr>
	</table>

</div>

