<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
User user = (User)request.getAttribute("User");
List<UserLogRecord> userLogRecords = (List<UserLogRecord>)request.getAttribute("UserLogRecords");
%>
	<div class="container_box">
	
		<div class="userPortalContent">

			<div class="userPortalContentHeader">
				<h2>Player Portal</h2>
			</div>
			<br/>
			
			<span class="userPortalBigBold">
				Welcome, <%=user.getUserName()%>
			</span>
			
			<br/>
			
<%
for(UserLogRecord userLogRecord : userLogRecords) {
%>
			<a href="./player-portal-player-log/<%=userLogRecord.getId()%>/">
				<%=MiscUtil.formatDateEEEEcsMMMsDsYYYY(userLogRecord.getRecordDate())%><br/>
			</a>
<%
}
%>
		
		</div>
		<div style="clear: both;"></div>
	</div> <!-- End of Container Box -->

