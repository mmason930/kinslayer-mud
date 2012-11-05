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
			
			Display Type: <a href="<%=normalLink%>">Normal</a> | <a href="<%=forumColoredLink%>">Forum Colored</a> | <a href="<%=webColoredLink%>">Web Colored</a>
			
			<%= userLog.getConsoleOutput() %>
		
		</div>
		<div style="clear: both;"></div>
	</div> <!-- End of Container Box -->

