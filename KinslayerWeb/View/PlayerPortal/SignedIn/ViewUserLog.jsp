<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
UserLog userLog = (UserLog)request.getAttribute("UserLog");
String normalLink, forumColoredLink, webColoredLink;

normalLink = WebSiteUrlUtil.getUserLogUrl(webSupport.getInstanceDomain(), userLog.getId(), false, false);
forumColoredLink = WebSiteUrlUtil.getUserLogUrl(webSupport.getInstanceDomain(), userLog.getId(), false, true);
webColoredLink = WebSiteUrlUtil.getUserLogUrl(webSupport.getInstanceDomain(), userLog.getId(), true, false);
%>

<script type="text/javascript">
$(document).ready(function() {

	$("#selectAllButton").bind("click", function(event) {

		selectText("playerLogBuffer");
		event.preventDefault();
	});
});
</script>

	<div class="container_box">
	
		<div class="userPortalContent">
			
			<span class="userPortalBreadcrumbs">
			<a href="./player-portal-home">Player Portal Home</a> &gt; Player Log
			</span>

			<div class="textCentered">
				<span class="userPortalUserLogDisplayTypeBox">
					Display Type: <a href="<%=normalLink%>">Normal</a> | <a href="<%=forumColoredLink%>">Forum Colored</a> | <a href="<%=webColoredLink%>">Web Colored</a>
				</span>
			</div>
			<br/>
			<br/>

			<div class="userPortalContentHeader">
				<h2>Player Log</h2>
				<span class="bigBold"><%=MiscUtil.formatDateEEEEcsMMMsDsYYYY(userLog.getRecordDate())%></span>
			</div>
			<br/>
			
			<br/>
			
			<a href="#" id="selectAllButton">Select All</a>
			<br/>
			
			<div id="playerLogBuffer">
				<%= userLog.getConsoleOutput() %>
			</div>
		
		</div>
		<div style="clear: both;"></div>
	</div> <!-- End of Container Box -->

