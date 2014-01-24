<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
List<String> errors = (List<String>)request.getAttribute("FormErrors");
%>
        <div class="container_box">
        
                <div class="userPortalContent">
                
                        <form method="POST" action="./player-portal-login" id="userPortalLoginForm">
<%
if(errors != null && errors.size() > 0) {
%>
								<ul>
<%
  for(String error : errors) {
%>
									<li><%=error%></li>
<%
  }
%>
								</ul>
<%
}
%>
                                <div class="userPortalInputField">Username:</div>
                                <input type="text" name="Username" /><br/>
                                
                                <div class="userPortalInputField">Password:</div>
                                <input type="password" name="Password" /><br/>
                                
                                <button class="userPortalLoginButton" type="submit">SIGN IN</button>
                        </form>
                
                </div>
                <div style="clear: both;"></div>
        </div> <!-- End of Container Box -->
