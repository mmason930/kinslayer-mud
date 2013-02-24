<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%@ page import="org.kinslayermud.web.equipment.EquipmentCatalogAction.*" %>
<%

EquipmentListingType equipmentListingType = (EquipmentListingType)request.getAttribute("EquipmentListingType");
List<LeftNavListItem> leftNavListItems = (List<LeftNavListItem>)request.getAttribute("LestNavListItems");
List<ObjectPrototype> objectPrototypes = (List<ObjectPrototype>)request.getAttribute("ObjectPrototypes");
%>
		<div id="pageContent" class="autoWidth">


<style type="text/css">

#equipmentOrWeaponPanel
{
	width: 60%;
	text-align: center;
	margin:auto;
}

#equipmentOrWeaponPanel div
{
	float: left;
	width: 50%;
}

#equipmentOrWeaponPanel div a
{
	text-decoration: none;
	color: #FFF;
	font-size: 24px;
}


#equipmentOrWeaponPanel div a:hover,
#equipmentOrWeaponPanel div a.selected
{
	text-decoration: underline;
	color: #880015;
}

.equipmentLeftNav
{
	float: left;
	padding-right: 1%;
	width: 17%;
	font-size: 75%;
	margin-left: 4%;
}

.equipmentLeftNav ul
{
	padding: 0 0 0 0;
	margin: 0 0 0 8%;
}

.equipmentLeftNav ul li
{
	list-style: none;
}


.equipmentLeftNav ul li a
{
	float: left;
	color: #FFF;
	width: 100%;
	line-height: 32px;
	text-decoration: none;
	font-size: 17px;
}

.equipmentLeftNav ul li a.selected,
.equipmentLeftNav ul li a:hover
{
	color: #880015 !important;
}

.equipmentRightPanel
{
	float: left;
	width: 77.5%;
}

.equipmentArmorTableContainer
{
	margin-top: 5%;
	width: 87%;
	float: left;
}

.equipmentArmorTableContainerInner
{
	border: 1px solid #FFF;
}

.equipmentArmorTableContainerInner table
{
	width: 100%;
	font-weight: normal;
}


.equipmentArmorTableContainerInner table tr th,
.equipmentArmorTableContainerInner table tr td
{
	font-size: 16px;
}

.equipmentArmorTableContainerInner table tr th.itemNameColumn,
.equipmentArmorTableContainerInner table tr td.itemNameColumn,
.equipmentArmorTableContainer .itemNameColumn
{
	width: 60%;
	text-align: left;
}

.equipmentArmorTableContainerInner table tr th.dodgeColumn,
.equipmentArmorTableContainerInner table tr td.dodgeColumn,
.equipmentArmorTableContainer .dodgeColumn
{
	width: 10%;
	text-align: center;
}
.equipmentArmorTableContainerInner table tr th.parryColumn,
.equipmentArmorTableContainerInner table tr td.parryColumn,
.equipmentArmorTableContainer .parryColumn
{
	width: 10%;
	text-align: center;
}
.equipmentArmorTableContainerInner table tr th.absColumn,
.equipmentArmorTableContainerInner table tr td.absColumn,
.equipmentArmorTableContainer .absColumn
{
	width: 10%;
	text-align: center;
}

.equipmentArmorTableContainerInner table tr th.weightColumn,
.equipmentArmorTableContainerInner table tr td.weightColumn,
.equipmentArmorTableContainer .weightColumn
{
	width: 10%;
	text-align: center;
}



.equipmentWeaponTableContainer
{
	margin-top: 5%;
	width: 87%;
	float: left;
}

.equipmentWeaponTableContainerInner
{
	border: 1px solid #FFF;
}

.equipmentWeaponTableContainerInner table
{
	width: 100%;
	font-weight: normal;
}


.equipmentWeaponTableContainerInner table tr th,
.equipmentWeaponTableContainerInner table tr td
{
	font-size: 16px;
}

.equipmentWeaponTableContainerInner table tr th.itemNameColumn,
.equipmentWeaponTableContainerInner table tr td.itemNameColumn,
.equipmentWeaponTableContainer .itemNameColumn
{
	width: 60%;
	text-align: left;
}

.equipmentWeaponTableContainerInner table tr th.offensiveColumn,
.equipmentWeaponTableContainerInner table tr td.offensiveColumn,
.equipmentWeaponTableContainer .offensiveColumn
{
	width: 13.33%;
	text-align: center;
}
.equipmentWeaponTableContainerInner table tr th.parryColumn,
.equipmentWeaponTableContainerInner table tr td.parryColumn,
.equipmentWeaponTableContainer .parryColumn
{
	width: 13.33%;
	text-align: center;
}
.equipmentWeaponTableContainerInner table tr th.weightColumn,
.equipmentWeaponTableContainerInner table tr td.weightColumn,
.equipmentWeaponTableContainer .weightColumn
{
	width: 13.33%;
	text-align: center;
}

</style>
			<div id="equipmentOrWeaponPanel">

				<div><a href="<%=WebSiteUrlUtil.getEquipmentListingUrl(webSupport.getInstanceDomain(), EquipmentListingType.armor , null, null) %>" <%=equipmentListingType != null && equipmentListingType.equals(EquipmentListingType.armor) ? ("class='selected'") : "" %>><%=EquipmentListingType.armor.getPluralName() %></a></div>
				<div><a href="<%=WebSiteUrlUtil.getEquipmentListingUrl(webSupport.getInstanceDomain(), EquipmentListingType.weapon, null, null) %>" <%=equipmentListingType != null && equipmentListingType.equals(EquipmentListingType.weapon)    ? ("class='selected'") : "" %>><%=EquipmentListingType.weapon.getPluralName() %></a></div>
				<div class="clearLeft"></div>
			</div>

			<div id="contentInner">

				<!-- LEFT PANEL -->
				<div class="equipmentLeftNav">
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