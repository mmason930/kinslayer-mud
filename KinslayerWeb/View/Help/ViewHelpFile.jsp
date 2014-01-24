<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%@ page import='org.kinslayermud.web.help.ViewHelpFileAction.*' %>
<%
ViewHelpFileData viewHelpFileData = (ViewHelpFileData)request.getAttribute("ViewHelpFileData");
request.setAttribute("SelectedHelpFile", viewHelpFileData.helpFile);
request.setAttribute("HelpFileChain", viewHelpFileData.helpFileChain);
%>
<h1 class="helpHeader"><%=viewHelpFileData.h1 %></h1>
<div class="subcategories">
	<div class="subcategoriesInner">
		<div class="selectedCategory">
			<span><%=viewHelpFileData.currentCategoryText %></span>
			<div class="underline"></div>
		</div>
		<ul>
<%
for(HelpFile childHelpFile : viewHelpFileData.childrenHelpFiles) {
  String url = WebSiteUrlUtil.getHelpFileUrl(webSupport.getInstanceDomain(), viewHelpFileData.helpFileChain, childHelpFile);
%>
			<li><a href="<%=url%>"><%=childHelpFile.getName() %></a></li>
<%
}
%>
		</ul>
	</div>
</div>
<div class="descriptionWrapper">
	<div class="breadcrumbs">
<%
if(viewHelpFileData.helpFile != null) {
%>
		<tiles:insertTemplate template='/View/Help/Framework/Breadcrumbs.jsp'/>
<%
}
%>
	</div>
	
	<div class="description">
		<%=viewHelpFileData.description %>
	</div>
</div>