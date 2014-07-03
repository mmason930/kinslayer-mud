<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<tiles:useAttribute name="replaceBannerImage" classname="java.lang.String" />
<%
String sessionUserName = (String)request.getAttribute("SessionUserName");
Integer sessionUserId = (Integer)request.getAttribute("SessionUserId");
String signInMessage = "Sign In";
String signInUrl = WebSiteUrlUtil.getPlayerPortalSignInUrl();
if(sessionUserName != null && sessionUserId != null) {

  signInMessage = "Sign Out [" + sessionUserName + "]";
  signInUrl = WebSiteUrlUtil.getPlayerPortalSignOutUrl();
}
%>
		<tiles:insertTemplate template="/View/Framework/LeftNav.jsp"/>
	
		<div class="rightSideContainer">
			<div class="topRight">
			<a href="./mudclient/mudclient.php" rel="nofollow">Play Now</a> |
			<a href="<%=signInUrl %>"><%=signInMessage %></a> |
<%
if(sessionUserId != null) {
%>
<a href="<%=WebSiteUrlUtil.getPlayerPortalHomeUrl()%>">My Account</a> |
<%
}
%>
Host: kinslayermud.org | Port: 2222</div>

<%
if(StringUtil.removeNull(replaceBannerImage).equals("true")) {
%>
			<div class="bodyContainer">
				<tiles:insertAttribute name="body" />
			</div>
<%
}
else {
%>
			<img class="bannerImage" src="./images/sword_on_black_surface_and_background-wide-no-margins.jpg"></img>
<%
}
%>
		</div>
		<div class="clearLeft"></div>
