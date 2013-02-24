<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
User user = (User)request.getAttribute("User");
%>
	<div class="container_box">
	
		<div class="userPortalContent">

			Level: <%=user.getLevel() %><br/>
			Race: <%=user.getUserRace().getStandardName() %><br/>
			Class: <%=user.getUserClass().getStandardName() %><br/>
<%
if(user.getLastLogon() != null) {
%>
			Last Logged In: <%= MiscUtil.formatDateMMMMsDucsYYYYathcMMsAM(user.getLastLogon()) %><br/>
<%
}
if(user.getLastLogout() != null) {
%>
			Last Logged Out: <%= MiscUtil.formatDateMMMMsDucsYYYYathcMMsAM(user.getLastLogout()) %><br/>
<%
}
%>
		
		</div>
		<div style="clear: both;"></div>
	</div> <!-- End of Container Box -->

