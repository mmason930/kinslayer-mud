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
			
			A USER LOG IS HERE!
		
		</div>
		<div style="clear: both;"></div>
	</div> <!-- End of Container Box -->

