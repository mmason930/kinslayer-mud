<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
User user = (User)request.getAttribute("User");
List<UserLogin> userLogins = (List<UserLogin>)request.getAttribute("UserLogins");
List<List<UserLogin>> userLoginTable = (List<List<UserLogin>>)request.getAttribute("UserLoginTable");
%>
<table class="userPortalLoginHistory">
<%
for(List<UserLogin> userLoginRow : userLoginTable) {
%>
	<tr>
<%
  for(UserLogin userLogin : userLoginRow) {
    if(userLogin != null) {
%>
		<td><%=MiscUtil.formatDateMMMsDDcsYYYYsHHsMMsSS(userLogin.getLoginDatetime())%> [<%=userLogin.getHost() %>]</td>
<%
    }
  }
%>
	</tr>
<%
}
%>
</table>
