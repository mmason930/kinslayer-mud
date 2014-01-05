<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
User user = (User)request.getAttribute("User");
String sessionId = (String)request.getAttribute("SessionId");
%>
<form id="helpFileLoadForm">
	ID: <input type="text" id="helpFileId"/> <button type="submit" class="flatGreyButton" id="helpFileLoadButton">Load</button>
</form>
<form id="helpFileForm">
	<div class="formEntry clearFix"><label>ID:</label>123456</div>
	<div class="formEntry clearFix"><label>Name:</label><input type="text" id="helpFileName"/></div>
	<div class="formEntry clearFix"><label>Syntax:</label><input type="text" id="helpFileSyntax"/></div>
	<div class="formEntry clearFix"><label>Keywords:</label><input type="text" id="helpFileKeywords"/></div>
	<div class="formEntry clearFix"><label class="floatLeft">Description:</label><div id="helpFileEditor"></div></div>

	<button type="submit" class="flatGreyButton floatRight" id="helpFileSaveButton">Save</button>
</form>
			
<script src="./View/JavaScript/ace-min/ace.js" type="text/javascript" charset="utf-8"></script>
<script>
	var editor = ace.edit("helpFileEditor");
	editor.setTheme("ace/theme/monokai");
	editor.getSession().setMode("ace/mode/javascript");
</script>

<script type="text/javascript">

var socket = new WebSocket("ws://kinslayermud.org:5002", "player-portal-protocol");
var inputBuffer = "";

socket.onopen = function()
{
	console.log("Connected.");
	inputBuffer = "";
	
	this.sendCommand({
		method: "SessionID",
		userId: <%= user.getUserId() %>
		sessionId: <%= sessionId %>
	})
}
socket.onclose = function()
{
	console.log("Disconnected.");
}
socket.onmessage = function(msg)
{
	client.inputBuffer += msg.data;
	console.log("Message Received: " + msg);
	
	var command = JSON.parse(msg.data);
	
	console.log("Message Method: " + command.method);
}

socket.sendCommand = function(command)
{
	this.send(JSON.stringify(command) + String.fromCharCode(0x06));
}

</script>