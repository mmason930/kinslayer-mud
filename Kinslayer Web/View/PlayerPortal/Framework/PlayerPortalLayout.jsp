<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<tiles:useAttribute name="topNavSelected" classname="java.lang.String" />
<tiles:useAttribute name="playerPortalH2" classname="java.lang.String" />
<tiles:useAttribute name="hideTopNav" classname="java.lang.String" ignore="true" />
<%
String sessionUserName = (String)request.getAttribute("SessionUserName");
boolean hideTopNavBoolean = StringUtil.removeNull(hideTopNav).equals("true");
User user = (User)request.getAttribute("User");
%>
<div class="container_box">

		<div class="userPortalContent">

			<div class="userPortalContainer">
<%
if(!hideTopNavBoolean) {
%>
				<ul class="userPortalTopNav">
					<li><a href="<%=WebSiteUrlUtil.getPlayerPortalHomeUrl()%>" <%=topNavSelected.equals("Home") ? "class='selected'" : "" %>>Home</a></li>
					<li><a href="<%=WebSiteUrlUtil.getPlayerPortalViewUserLogs() %>" <%=topNavSelected.equals("UserLogs") ? "class='selected'" : "" %>>User Logs</a></li>
					<li><a href="<%=WebSiteUrlUtil.getPlayerPortalViewLoginHistoryUrl() %>" <%=topNavSelected.equals("ViewLoginHistory") ? "class='selected'" : "" %>>Login History</a></li>
					<li><a href="<%=WebSiteUrlUtil.getPlayerPortalViewTrophiesUrl() %>" <%=topNavSelected.equals("ViewTrophies") ? "class='selected'" : "" %>>Trophies</a></li>
					<li><a href="<%=WebSiteUrlUtil.getPlayerPortalViewTellHistoryUrl(null) %>" <%=topNavSelected.equals("ViewTellHistory") ? "class='selected'" : "" %>>Tell History</a></li>
<%
  if(user.getLevel() >= 100) {
%>
					<li><a href="<%=WebSiteUrlUtil.getPlayerPortalHelpEditorUrl() %>" <%=topNavSelected.equals("HelpEditor") ? "class='selected'" : "" %>>Help Editor</a></li>
					<li><a href="<%=WebSiteUrlUtil.getAdminOlistUrl() %>">Admin</a></li>
<%
  }
%>
				</ul>
<%
}
%>
				<div class="clearLeft"></div>
				<div class="userPortalContainerInner">
					<div class="userPortalWelcome"><%=sessionUserName == null ? "" : ("Welcome, " + sessionUserName)%></div>
					<div class="userPortalContentHeader"><h2>Player Portal - <%=playerPortalH2 %></h2></div>
					<div class="clearLeft"></div>

					<tiles:insertAttribute name="content"/>

				</div>
			</div>

		</div>
		<div style="clear: both;"></div>
	</div> <!-- End of Container Box -->