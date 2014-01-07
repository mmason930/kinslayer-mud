<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
User user = (User)request.getAttribute("User");
System.out.println("User: " + user);
List<List<UserLogRecord>> userLogRecordsTable = (List<List<UserLogRecord>>)request.getAttribute("UserLogRecordsTable");
System.out.println("User Log Record Table: " + userLogRecordsTable);
%>
<table class="userLogTable">
<%
for(List<UserLogRecord> row : userLogRecordsTable) {
	System.out.println("Row: " + row);
%>
	<tr>
<%
  for(UserLogRecord userLogRecord : row) {
	  System.out.println("Record: " + userLogRecord);
    String timestamp = MiscUtil.formatDateMMMMsDDcsYYYY(userLogRecord.getRecordDate());
    String url = WebSiteUrlUtil.getUserLogUrl(webSupport.getInstanceDomain(), userLogRecord.getId(), true, false);
%>
		<td><a href="<%=url %>"><%=timestamp %></a></td>
<%
  }
%>
	</tr>
<%
}
%>
</table>