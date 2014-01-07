<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
User user = (User)request.getAttribute("User");
List<List<UserLogRecord>> userLogRecordsTable = (List<List<UserLogRecord>>)request.getAttribute("UserLogRecordsTable");
%>
<table class="userLogTable">
<%
for(List<UserLogRecord> row : userLogRecordsTable) {
%>
	<tr>
<%
  for(UserLogRecord userLogRecord : row) {
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