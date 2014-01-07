<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
User user = (User)request.getAttribute("User");
String sessionId = (String)request.getAttribute("SessionId");
%>
<script type="text/javascript">

var socket = null;
var commandProcessors = {};

commandProcessors["Load Help File"] = function(command)
{
	if(command.error && !socket.loadingParent) {
		$("#loadHelpFileError").text(command.error).removeClass("hidden");
		return;
	}

	if(!socket.loadingParent)
	{
		socket.setHelpFileForm(command);
	}
	else
	{
		socket.setParentHelpFile(command.helpFileId, command.name);
		socket.hideSetNewParentHelpFile();
	}

	socket.loadingParent = false;
};

commandProcessors["Save Help File"] = function(command)
{
	if(command.errors)
	{
		socket.displaySaveFormWarnings(command.errors);
		return;
	}

	socket.setHelpFileForm(command);
};

$(document).ready(function() {
	socket = new WebSocket("ws://kinslayermud.org:<%=webSupport.getPlayerPortalServerPort()%>", "player-portal-protocol");

	socket.onopen = function()
	{
		socket.inputBuffer = "";
		
		this.sendCommand({
			method: "SessionID",
			userId: <%= user.getUserId() %>,
			sessionId: "<%= sessionId %>"
		})
	}
	socket.onclose = function()
	{
	}
	socket.onmessage = function(msg)
	{
		var command = JSON.parse(msg.data);
		
		var commandProcessor = commandProcessors[command.method];
		if(commandProcessor)
		{
			commandProcessor(command);
		}
	}

	socket.sendCommand = function(command)
	{
		this.send(JSON.stringify(command) + String.fromCharCode(0x06));
	}

	socket.loadingParent = false;

	socket.setParentHelpFile = function(id, name)
	{
		$("#helpFileParentAnchor").text(name).data("help-file-id", id);
	}

	socket.hideSetNewParentHelpFile = function()
	{

		$("#helpFileChangeParentWrapper").addClass("hidden");
		$("#helpFileParentWrapper").removeClass("hidden");
	}

	socket.displaySaveFormWarnings = function(errors)
	{
		var $ul = $("#helpFileForm > ul");
		$ul.html("");
		errors.forEach(function(error) {
			var $li = $("<li></li>");
			$li.append(error);
			$ul.append($li);
		});

		$ul.removeClass("hidden");
	}

	socket.hideSaveFormWarnings = function()
	{
		$("#helpFileForm > ul").addClass("hidden");
	}

	socket.setHelpFileForm = function(command)
	{

		$("#loadHelpFileError").text("").addClass("hidden");
	
		$("#helpFileName").val(command.name == null ? "" : command.name);
		$("#helpFileSyntax").val(command.syntax == null ? "" : command.syntax);
		$("#helpFileKeywords").val(command.keywords == null ? "" : command.keywords);
		$("#helpFileIdValue").text(command.helpFileId);
		$("#helpFileLastEdited").text(command.lastModified);
		$("#helpFileCreated").text(command.created);
		socket.setParentHelpFile(command.parent ? command.parent.helpFileId : "", command.parent ? command.parent.name : "<None>");

		$("#helpFileChildren").html("");

		command.children.forEach(function(child, index) {

			var $a = $("<a></a>");
			$a.append(child.name).attr("href", "#").data("help-file-id", child.helpFileId);

			$("#helpFileChildren").append($a).append(index == command.children.length - 1 ? "" : ", ");
		});

		editor.selectAll();
		editor.removeLines();
		editor.insert(command.description);
	};
	
	$("#helpFileForm").on("submit", function(e) {
		e.preventDefault();

		var helpFileId = $("#helpFileIdValue").text();
		var helpFileName = $.trim($("#helpFileName").val());
		var helpFileSyntax = $.trim( $("#helpFileSyntax").val() );
		var helpFileKeywords = $.trim( $("#helpFileKeywords").val() );
		var helpFileDescription = editor.getValue();
		var helpFileParentId = $("#helpFileParentAnchor").data("help-file-id");
		var errors = [];

		if($.isNumeric(helpFileId))
			helpFileId = parseInt(helpFileId);
		else
			helpFileId = null;

		if(helpFileName == null || helpFileName == "")
			errors.push("Name must not be blank.");
		else if(helpFileName.length >= 60)
			errors.push("Name must be no longer than 60 characters.");

		if(helpFileSyntax.length >= 120)
			errors.push("Syntax must be no longer than 120 characters.");

		if(errors.length > 0)
		{
			socket.displaySaveFormWarnings(errors);
			return;
		}

		socket.hideSaveFormWarnings();

		var command = {
			method: "Save Help File",
			helpFileId: helpFileId,
			name: helpFileName,
			syntax: helpFileSyntax,
			keywords: helpFileKeywords,
			description: helpFileDescription,
			parentId: helpFileParentId == "" ? null : parseInt(helpFileParentId)
		};

		socket.sendCommand(command);
	})

	$("#helpFileLoadForm").on("submit", function(e) {
		
		e.preventDefault();
		socket.hideSetNewParentHelpFile();
		var helpFileId = $("#helpFileId").val();
		
		socket.sendCommand({
			method: "Load Help File",
			helpFileId: helpFileId
		});
	});

	$("#helpFileCreateNewButton").on("click", function(e) {

		e.preventDefault();

		socket.hideSetNewParentHelpFile();

		$("#loadHelpFileError").text("").addClass("hidden");
		$("#helpFileName").val("");
		$("#helpFileSyntax").val("");
		$("#helpFileKeywords").val("");
		$("#helpFileIdValue").text("<NEW>");
		$("#helpFileParentAnchor").text("<NONE>").data("help-file-id", "");
		$("#helpFileChildren").html("");
		$("#helpFileCreated").text("");
		$("#helpFileLastEdited").text("");

		editor.selectAll();
		editor.removeLines();
	});

	$("#helpFileChangeParentAnchor").on("click", function(e) {

		e.preventDefault();

		$("#helpFileChangeParentWrapper").removeClass("hidden");
		$("#helpFileParentWrapper").addClass("hidden");
		$("#helpFileNewParentIdInput").val("").focus();
	});

	$("#helpFileSaveNewParentIdButton").on("click", function(e) {

		e.preventDefault();

		var helpFileId = $("#helpFileNewParentIdInput").val();

		if(helpFileId == "")
		{
			socket.setParentHelpFile("", "<None>");
			socket.hideSetNewParentHelpFile();
			return;
		}

		if(!$.isNumeric(helpFileId) || parseInt(helpFileId) != helpFileId) {
			alert("Help File ID must either be blank(for no parent) or an integer");
			return;
		}

		var command = {
			method: "Load Help File",
			helpFileId: parseInt(helpFileId)
		};

		socket.loadingParent = true;
		socket.sendCommand(command);
	});

	$("#helpFileDiscardNewParentIdButton").on("click", function(e) {

		e.preventDefault();
		socket.hideSetNewParentHelpFile();
	});

	$("#helpFileChildren").on("click", "a", function(e) {

		e.preventDefault();

		var helpFileId = $(e.target).data("help-file-id");

		if(helpFileId == "" || helpFileId == null)
			return;

		socket.sendCommand({
			method: "Load Help File",
			helpFileId: parseInt(helpFileId)
		});
	});

	$("#helpFileParentAnchor").on("click", function(e) {

		e.preventDefault();

		var helpFileId = $(e.target).data("help-file-id");

		if(helpFileId == "" || helpFileId == null)
			return;

		socket.sendCommand({
			method: "Load Help File",
			helpFileId: parseInt(helpFileId)
		});
	});
});

</script>

		<div class="userPortalContent">

			<form id="helpFileLoadForm">
				ID or Name: <input type="text" id="helpFileId"/>
				<button type="submit" class="flatGreyButton" id="helpFileLoadButton">Load</button>
				<button type="button" class="flatGreyButton" id="helpFileCreateNewButton">Create New</button>
				<span class="hidden" id="loadHelpFileError"></span>
			</form>
			<form id="helpFileForm">
				<ul class="hidden">Errors.</ul>
				<div class="formEntry clearFix"><label>ID:</label><span id="helpFileIdValue">&lt;NEW&gt;</span></div>
				<div class="formEntry clearFix"><label>Name:</label><input type="text" id="helpFileName"/></div>
				<div class="formEntry clearFix"><label>Syntax:</label><input type="text" id="helpFileSyntax"/></div>
				<div class="formEntry clearFix"><label>Keywords:</label><input type="text" id="helpFileKeywords"/></div>
				<div class="formEntry clearFix"><label>Parent:</label><div id="helpFileParentWrapper"><a href="#" id="helpFileParentAnchor" data-help-file-id="">&lt;NONE&gt;</a><a href="#" id="helpFileChangeParentAnchor">(change)</a></div><div id="helpFileChangeParentWrapper" class="hidden"><input type="text" id="helpFileNewParentIdInput"/> <button type="button" class="flatGreyButton short" id="helpFileSaveNewParentIdButton">Save</button> <button type="button" class="flatGreyButton short" id="helpFileDiscardNewParentIdButton">Discard</button></div></div>
				<div class="formEntry clearFix"><label>Children:</label><span id="helpFileChildren"></span></div>
				<div class="formEntry clearFix"><label>Created:</label><span id="helpFileCreated"></span></div>
				<div class="formEntry clearFix"><label>Last Edited:</label><span id="helpFileLastEdited"></span></div>
				<div class="formEntry clearFix"><label class="floatLeft">Description:</label><div id="helpFileEditor"></div></div>

				<button type="submit" class="flatGreyButton marginAuto" id="helpFileSaveButton">Save</button>
			</form>

		</div>
		<div style="clear: both;"></div>
	</div> <!-- End of Container Box -->

<script src="./View/JavaScript/ace-min/ace.js" type="text/javascript" charset="utf-8"></script>
<script>
    var editor = ace.edit("helpFileEditor");
    editor.setTheme("ace/theme/monokai");
    editor.getSession().setMode("ace/mode/javascript");
    editor.getSession().setUseWrapMode(true);
</script>