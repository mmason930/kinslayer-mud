<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
User user = (User)request.getAttribute("User");
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