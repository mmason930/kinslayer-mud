<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
UserLog userLog = (UserLog)request.getAttribute("UserLog");
%>
<html>

	<body>

<form action="Admin.LoadUserLog.do" method="POST">
Load User Log: <input type="text" size="20" name="UserLogID" /> <input type="submit" value="Load"/><br/><br/>
</form>

		<form method="POST" action="/Admin.SaveUserLog.do">

			User Log ID: <input name="UserLogID" type="text" size="20" value="<%=userLog == null ? "" : String.valueOf(userLog.getId())%>"/><br/>
			<textarea rows="15" cols="175" name="ConsoleOutput"><%=userLog == null ? "" : userLog.getConsoleOutput() %></textarea><br/>
			<input type="submit" value="Submit"/>
			
		</form>

	</body>

</html>