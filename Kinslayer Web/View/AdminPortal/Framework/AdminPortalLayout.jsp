<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<tiles:useAttribute name="title" classname="java.lang.String" ignore="true" />
<%
String sessionUserName = (String)request.getAttribute("SessionUserName");
User user = (User)request.getAttribute("User");
if(request.getAttribute("Title") != null) {
  title = (String)request.getAttribute("Title");
}
%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
	<head>
		<title><%=title %></title>
		<meta http-equiv="Content-Language" content="EN" />
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		<base href="http://<%=webSupport.getInstanceDomain() %>/" />
		<link rel="stylesheet" href="./View/Styles/style.css" type="text/css"/>
		<link rel="stylesheet" href="./View/Styles/PlayerPortalStyles.css" type="text/css"/>
		<link rel="stylesheet" href="./View/Styles/Admin/OList.css" type="text/css"/>
		<link rel="stylesheet" href="./View/Styles/HelpStyles.css" type="text/css"/>
		<script type="text/javascript" src="./View/JavaScript/jquery-1.8.0.js"></script>
		<script type="text/javascript" src="./View/JavaScript/scripts.js"></script>
	</head>

	<body>
		<tiles:insertAttribute name="body" />
	</body>
</html>


