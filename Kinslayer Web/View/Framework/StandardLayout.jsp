<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
 <head>
  <title><tiles:insertAttribute name="title" ignore="true" /></title>
  <base href="<%=(request.isSecure() ? "https://" : "http://") + request.getServerName() + request.getContextPath() %>/View/" />
  <script type="text/javascript" src="JavaScript/jquery-1.8.0.js"></script>
  <link rel="stylesheet" type="text/css" href="Styles/styles01.css" media="screen" />
 </head>
 <body>
  <div class="floatLeft" style="width: 15%">&nbsp;</div>
  <div class="mainContent">
   <div class="mainContentTop">&nbsp;</div>
   <div class="mainContentInner">
    <tiles:insertAttribute name="body" />
   </div>
   <div class="mainContentBottom">&nbsp;</div>
  </div>
 </body>
</html>