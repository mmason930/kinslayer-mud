<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
UserLog userLog = (UserLog)request.getAttribute("UserLog");
String normalLink, forumColoredLink, webColoredLink;

normalLink = WebSiteUrlUtil.getUserLogUrl(webSupport.getInstanceDomain(), userLog.getId(), false, false);
forumColoredLink = WebSiteUrlUtil.getUserLogUrl(webSupport.getInstanceDomain(), userLog.getId(), false, true);
webColoredLink = WebSiteUrlUtil.getUserLogUrl(webSupport.getInstanceDomain(), userLog.getId(), true, false);
%>
	<div class="container_box">
	
		<div class="userPortalContent">

			<div class="userPortalContentHeader">
				<h2>Player Portal</h2>
			</div>
			<br/>
			
			<div class="textCentered">
				<span class="userPortalUserLogDisplayTypeBox">
					Display Type: <a href="http://dev.kinslayermud.org/player-portal-view-user-log/7/">Normal</a> | <a href="http://dev.kinslayermud.org/player-portal-view-user-log/7/?DisplayType=Forum">Forum Colored</a> | <a href="http://dev.kinslayermud.org/player-portal-view-user-log/7/?DisplayType=Html">Web Colored</a>
				</span>
			</div>
			
			<br/>
			<%= userLog.getConsoleOutput() %>
		
		</div>
		<div style="clear: both;"></div>
	</div> <!-- End of Container Box -->

