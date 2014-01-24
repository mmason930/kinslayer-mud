<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<html>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
	<head>
		<meta http-equiv="Content-Language" content="EN" /> 
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8"> 
		<meta http-equiv="content-type" content="text/html;charset=iso-8859-2" />
		<title>KinslayerMUD - Play Now!</title>
		<base href="http://<%=webSupport.getInstanceDomain() %>/" />
		<link rel="stylesheet" href="./View/Styles/web-socket-client-styles.css" type="text/css"/>
		<script type="text/javascript" src="./View/JavaScript/jquery-1.8.0.js"></script>
	<script type="text/javascript">
window.onbeforeunload = function()
{ 
	return "The MUD client is still running!";
}

var socket;
var commandHistory = [];
var commandHistoryPosition = 0;
var connected = false;
var connectedDatetime = null;
var onlineTimeInterval = setInterval(updateOnlineTime, 1000);
var outputBuffer = "";
var macros = new Object();//Associative array.

var outputCurrentStyle =
{
	color: null,
	bold: false
};

String.prototype.trim = function() {
	return this.replace(/^\s+|\s+$/g,"");
}
String.prototype.ltrim = function() {
	return this.replace(/^\s+/,"");
}
String.prototype.rtrim = function() {
	return this.replace(/\s+$/,"");
}

function zeroFill( number, width )
{
	width -= number.toString().length;
	if ( width > 0 )
		return new Array( width + (/\./.test( number ) ? 2 : 1) ).join( '0' ) + number;
	return number + "";
}

function updateOnlineTime() {

	if(!connected || connectedDatetime == null)
		return;
	
	var secondsDifference = parseInt(((new Date()).getTime() - connectedDatetime.getTime()) / 1000);
	
	var seconds = zeroFill(parseInt(secondsDifference % 60), 2);
	var minutes = zeroFill(parseInt((secondsDifference / 60) % 60), 2);
	var hours  = zeroFill(parseInt(secondsDifference / 60 / 60), 2);
	
	$("#connectionBox").text(hours + ":" + minutes + ":" + seconds);
}

function formatMUDOutputForWindow(outputReceived)
{
	outputReceived = outputReceived.replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/\n/g, "<br/>").replace(/\s/g, "&nbsp;").replace(/(\r\n|\n|\r)/gm, "");
	var regex = /\x1B\[(\d+)m/g
	var matches;
	
	outputReceived = outputReceived + generateOutputStyleSpan(outputCurrentStyle.color, outputCurrentStyle.bold);
	var outputFinal = "";
	var lastIndex = 0;
	while( (matches = regex.exec(outputReceived)) != null)
	{
		var colorCode = matches[ 1 ];
		
		outputFinal += outputReceived.substr(lastIndex, matches.index - lastIndex);
		
		if(outputCurrentStyle.color != null)
		{
			outputFinal += "</span>";
		}
//		if(colorCode == '36' && !outputCurrentStyle.bold)
//		{
//			outputCurrentStyle.bold = true;
//		}
		else
		{
			outputCurrentStyle.color = getHexColorCodeFromTelnetColorCode(colorCode);
		}
		
		outputFinal += generateOutputStyleSpan(matches[1]);
		lastIndex = matches.index + matches[ 0 ].length;
	}
	
	outputFinal += outputReceived.substr(lastIndex);//Copy the remainder.
	
	if(outputCurrentStyle.color != null)
	{
		outputFinal += "</span>";
	}
	
	return outputFinal;
}

function getHexColorCodeFromTelnetColorCode(telnetColorCode)
{
	if(telnetColorCode == '0')
		return "#ffffff";//Normal
	else if(telnetColorCode == '31')
		return "#800000";//red
	else if(telnetColorCode == '32')
		return '#00b300';//green
	else if(telnetColorCode == '33')
		return '#808000';//yellow
	else if(telnetColorCode == '34')
		return '#000080';//blue
	else if(telnetColorCode == '35')
		return '#800080';//magenta
	else if(telnetColorCode == '36')
		return '#008080';//cyan
	else if(telnetColorCode == '37')
		return '#ffffff';//white
	return null;
}

function generateOutputStyleSpan(currentColor, isBold)
{
	if(currentColor == null && !isBold)
		return "";
	return "<span style='" + (currentColor != null ? ("color:" + getHexColorCodeFromTelnetColorCode(currentColor) + ";") : "") + (isBold ? "font-weight:bold;" : "") + "'>";
}

function appendToOutputWindow(outputReceived)
{
	$('#outputWindow').append(outputReceived);
	var outputWindow = document.getElementById("outputWindowWrapper");
	outputWindow.scrollTop = outputWindow.scrollHeight;
}

function setSelectionRange(input, selectionStart, selectionEnd) {
  if (input.setSelectionRange) {
    input.focus();
    input.setSelectionRange(selectionStart, selectionEnd);
  }
  else if (input.createTextRange) {
    var range = input.createTextRange();
    range.collapse(true);
    range.moveEnd('character', selectionEnd);
    range.moveStart('character', selectionStart);
    range.select();
  }
}

function setCaretToPos (input, pos) {
  setSelectionRange(input, pos, pos);
}

function processSpecialCommand(commandJson)
{
	var method;
	var details = "";
	
	var commandObject = JSON.parse(commandJson);
	var method = commandObject.method;
	
	if(!method)
		return;
	
	if(method == "Username")
	{
		$("#usernameValue").text(commandObject.username);
	}
	else if(method == "Players Online")
	{
		$("#totalPlayersValue").text(commandObject.numberOfPlayers);
	}
}

//Returns true if it is okay to process the output buffer for display. False otherwise.
function processSpecialCommands()
{
	var hasCompletedCommand;
	var hasPartialCommand;
	
	var startSequence = String.fromCharCode(0x1B,0x06,0x1B);
	var endSequence = String.fromCharCode(0x06,0x1B,0x06);
	do
	{
		var index = outputBuffer.indexOf(String.fromCharCode(0x1B));
		hasCompletedCommand = false;
		hasPartialCommand = false;
		
		if(index != -1)
		{

			hasPartialCommand = true;
			if(outputBuffer.length > index + 1)
			{
				if(outputBuffer.charAt(index + 1) == String.fromCharCode(0x06))
				{//Second character
				
					if(outputBuffer.length > index + 2)
					{
						if(outputBuffer.charAt(index + 2) == String.fromCharCode(0x1B))
						{//Third character
						
							//Look for the end sequence. If we find it, then we can process the command.
							
							var endIndex = outputBuffer.indexOf(endSequence, index + 4);
							
							if(endIndex != -1)
							{

								hasCompletedCommand = true;
								var extractionStartIndex = index + 3;
								var command = outputBuffer.slice(extractionStartIndex, endIndex);
								
								//alert("Command: " + command);
								processSpecialCommand(command);

								var startOfNewOutputBuffer = outputBuffer.substring(0, index - 1);
								var endOfNewOutputBuffer = outputBuffer.substring(endIndex + 3);
								
								outputBuffer = startOfNewOutputBuffer + endOfNewOutputBuffer;
							}
						}
						else
							hasPartialCommand = false;
					}
				}
				else
					hasPartialCommand = false;
			}
		}
	}
	while(hasCompletedCommand == true);
	
	if(hasPartialCommand)
		return false;
	return true;
}

$(document).ready(function(){

	$("#inputWindow").select();
	if(typeof WebSocket == "undefined") {
	
		alert("This client is not supported by your browser. Please update to a modern browser.");
		return;
	}
	
	macros["103"] = "where";
	$("body").bind("keydown", function(event) {
	
		var macro = macros[event.which + ""];
		if(macro)
		{
			socket.send(macro + "\n");
			appendToOutputWindow("<span class=\"inputCommand\">" + macro + "</span><br/>");
			event.preventDefault();
		}
	});
	
	$("#macroShowHideAnchor").bind("click", function(event) {
		
		$("#macroConfiguration").toggleClass("hidden");
		event.preventDefault();
	});
	
	var protocol = "mud-protocol";
	socket = new WebSocket("ws://kinslayermud.org:4001", "lws-mirror-protocol");
//	socket = new WebSocket("ws://localhost:4001", "lws-mirror-protocol");
	
	socket.onopen = function() {
	
		connected = true;
		connectedDatetime = new Date();
		$("#connectionBox").css("background", "#00FF00");
	}
	//Executed when socket is closed
	socket.onclose = function() {
	
		connected = true;
		connectedDatetime = null;
		$("#connectionBox").text("Not Connected");
		$("#connectionBox").css("background", "#FF0000");
	}
	//Executed when a message is received through the socket
	socket.onmessage = function(msg) {
	//	alert("MESSAGE LENGTH: " + msg.data.length);
	
		var messageData = msg.data;
		
		outputBuffer += messageData;
		
		var canProcessOutputBuffer = processSpecialCommands();

		if(canProcessOutputBuffer) {

			var formattedMessage = formatMUDOutputForWindow(outputBuffer);
		
			appendToOutputWindow(formattedMessage);
			
		
			outputBuffer = new String("");
		}
	}

	$("#inputWindow").bind("keypress", function(event) {
	
		if(event.which == 13)
		{//Enter key pressed
			
			var input = $('#inputWindow').val();
			$("#inputWindow").select();
			socket.send(input + "\n");
			appendToOutputWindow("<span class=\"inputCommand\">" + input + "</span><br/>");
			event.preventDefault();
			
			commandHistory.push( input );//TODO: Add buffer size limitation
			commandHistoryPosition = commandHistory.length;
		}
	});
	
	$("#inputWindow").bind("keydown", function(event) {
	
		if(event.which == 38) {//Up
		
			if(commandHistoryPosition != 0) {//Can we scroll up any further?
				commandHistoryPosition -= 1;
				$("#inputWindow").val( commandHistory[ commandHistoryPosition ] );
				
				setCaretToPos( $("#inputWindow").get(0), commandHistory[ commandHistoryPosition ].length );
			}
			
			return false;
		}
		else if(event.which == 40) {//Down
		
			if(commandHistoryPosition != commandHistory.length) {//Can we scroll down any further?
				commandHistoryPosition += 1;
				$("#inputWindow").val( commandHistory[ commandHistoryPosition ] );
				setCaretToPos( $("#inputWindow").get(0), commandHistory[ commandHistoryPosition ].length );
			}
			else {//If not, clear the prompt.
			
				$("#inputWindow").val("");
			}
			
			return false;
		}
	});

});

		</script>
	</head>
	<body>
 
		<div id="mainContentWrapper">
			<div id="mainContentInner">
				<div id="outputWindowWrapper">
					<div id="outputWindow">
					</div>
				</div>    
				<div id="inputPanel">
					<input id="inputWindow" type="text" />
					<div id="connectionBox"></div>
				</div>
			</div>
		</div>

		<div id="rightPanelWrapper">
			<div id="rightPanel">
    
				<div class="totalPlayersRow">
					<span id="totalPlayersLabel" class="rightPanelLabel">Players Connected:</span>
					<span id="totalPlayersValue" class="rightPanelValue"></span>
				</div>
   
				<div class="usernameRow">
					<span id="usernameLabel" class="rightPanelLabel">Username:</span>
					<span id="usernameValue" class="rightPanelValue"></span>
				</div>
   
				<div id="macros">
					<a href="#" id="macroShowHideAnchor">Show Macro Configuration</a>
					<div id="macroConfiguration" class="hidden">
						<span>Key: <span> <input type="text" size="25" class="macroInput" name="newMacroKey" />
						<span>Replacement: <span> <input type="text" size="25" class="macroInput"  name="newMacroValue" />
					</div>
				</div>
			</div>
  
			<div class="clearLeft">
			</div>
		</div>
	</body>
</html>