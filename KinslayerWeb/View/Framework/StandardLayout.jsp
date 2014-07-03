<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<tiles:useAttribute name="leftNavSelected" classname="java.lang.String" />
<tiles:useAttribute name="MetaDescription" classname="java.lang.String" ignore="true" />
<tiles:useAttribute name="title" classname="java.lang.String" ignore="true" />
<tiles:useAttribute name="stylesheets" classname="java.lang.String" ignore="true" />
<tiles:useAttribute name="replaceBannerImage" classname="java.lang.String" />
<%
request.setAttribute("LeftNavSelected", StringUtil.removeNull(leftNavSelected));
if(request.getAttribute("MetaDescription") != null) {
  MetaDescription = (String)request.getAttribute("MetaDescription");
}
if(request.getAttribute("Title") != null) {
  title = (String)request.getAttribute("Title");
}

String[] stylesheetsArray = StringUtil.isNullOrEmptyTrimmedString(stylesheets) ? new String[0] : stylesheets.split(",");
%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
	<head>
		<title><%=title %></title>
		<meta http-equiv="Content-Language" content="EN" />
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<%
if(MetaDescription != null) {
%>
		<meta name="DESCRIPTION" content="<%=MetaDescription %>" />
<%
}
%>
		<base href="http://<%=webSupport.getInstanceDomain() %>/" />
		<link rel="stylesheet" href="./View/Styles/style.css" type="text/css"/>
		<link rel="stylesheet" href="./View/Styles/PlayerPortalStyles.css" type="text/css"/>
		<link rel="stylesheet" href="./View/Styles/HelpStyles.css" type="text/css"/>
<%
for(String stylesheetFileName : stylesheetsArray) {
%>
	<link rel="stylesheet" href="./View/Styles/<%=stylesheetFileName %>" type="text/css"/>	
<%
}
%>
		<script type="text/javascript" src="./View/JavaScript/jquery-1.8.0.js"></script>
		<script type="text/javascript" src="./View/JavaScript/scripts.js"></script>
	</head>
  
	<body>
		<tiles:insertTemplate template="/View/Framework/Header.jsp" />

<%
if(StringUtil.removeNull(replaceBannerImage).equals("false")) {
%>
		<tiles:insertAttribute name="body" />
<%
}
%>

		<div class="clearBoth"></div>
		<div style="margin-bottom: 1%;">&nbsp;</div>

		<tiles:insertTemplate template="/View/Framework/Footer.jsp" />

	</body>
</html>

