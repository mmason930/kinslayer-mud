<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%@ page import="org.kinslayermud.web.equipment.EquipmentCatalogAction.*" %>
<%

EquipmentListingType equipmentListingType = (EquipmentListingType)request.getAttribute("EquipmentListingType");
List<LeftNavListItem> leftNavListItems = (List<LeftNavListItem>)request.getAttribute("LestNavListItems");
List<ObjectPrototype> objectPrototypes = (List<ObjectPrototype>)request.getAttribute("ObjectPrototypes");
String pageContent = (String)request.getAttribute("PageContent");
String h1 = (String)request.getAttribute("H1");
%>
		<div id="pageContent" class="autoWidth">

			<div id="equipmentOrWeaponPanel">

				<div><a href="<%=WebSiteUrlUtil.getEquipmentListingUrl(webSupport.getInstanceDomain(), EquipmentListingType.armor , null, null) %>" <%=equipmentListingType != null && equipmentListingType.equals(EquipmentListingType.armor) ? ("class='selected'") : "" %>><%=EquipmentListingType.armor.getPluralName() %></a></div>
				<div><a href="<%=WebSiteUrlUtil.getEquipmentListingUrl(webSupport.getInstanceDomain(), EquipmentListingType.weapon, null, null) %>" <%=equipmentListingType != null && equipmentListingType.equals(EquipmentListingType.weapon)    ? ("class='selected'") : "" %>><%=EquipmentListingType.weapon.getPluralName() %></a></div>
				<div class="clearLeft"></div>
			</div>

			<div id="contentInner">

				<!-- LEFT PANEL -->
				<div class="equipmentLeftNav">&nbsp;
					<ul>
<%
if(leftNavListItems != null) {
  for(LeftNavListItem leftNavListItem : leftNavListItems) {

%>
						<li><a href="<%= leftNavListItem.getAnchorUrl() %>" <%=leftNavListItem.getIsSelected() ? "class='selected'" : "" %>><%=StringUtil.escapeHTMLCharacters(leftNavListItem.getAnchorDisplay()) %></a></li>
<%
  }
}
%>
					</ul>
				</div>

				<!-- RIGHT PANEL -->
				<div class="equipmentRightPanel">
				
<%
if(pageContent != null && h1 != null) {
%>
					<div class="equipmentDescriptionBox">
						<h1 class="header"><%=StringUtil.escapeHTMLCharacters(h1)%></h1>
						<%=pageContent %>
					</div>
<%
}
%>
<%
if(objectPrototypes.size() > 0) {
	
  if(equipmentListingType.equals(EquipmentListingType.armor)) {
%>
					<tiles:insertTemplate template="/View/Equipment/Framework/EquipmentCatalogArmorTable.jsp" />
<%
  }
  else if(equipmentListingType.equals(EquipmentListingType.weapon)) {
%>
					<tiles:insertTemplate template="/View/Equipment/Framework/EquipmentCatalogWeaponTable.jsp" />
<%
  }
}
%>
				
				</div>
				
			</div>

		</div>