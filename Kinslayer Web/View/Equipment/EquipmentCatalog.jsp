<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%@ page import="org.kinslayermud.web.equipment.EquipmentCatalogAction.*" %>
<%

EquipmentListingType equipmentListingType = (EquipmentListingType)request.getAttribute("EquipmentListingType");
List<LeftNavListItem> leftNavListItems = (List<LeftNavListItem>)request.getAttribute("LestNavListItems");
List<ObjectPrototype> objectPrototypes = (List<ObjectPrototype>)request.getAttribute("ObjectPrototypes");
%>
		<div id="pageContent" class="autoWidth">

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
				<div class="equipmentRightPanel">
				
					<div class="equipmentDescriptionBox">
						<h1 class="header">Neck Trinkets</h1>
						<p>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam eget leo arcu. Phasellus eget pretium massa. Etiam sed nulla et tellus gravida ornare sit amet sit amet ante. Mauris ultrices, enim gravida convallis posuere, urna dolor facilisis erat, vitae feugiat orci sapien vitae arcu. Sed lacinia aliquet ipsum quis euismod. Duis in mollis lacus. In tempus pretium dolor, et rhoncus orci vestibulum in. Curabitur nec nunc non nulla pellentesque congue.</p>
<!--
						<p>Praesent libero arcu, sollicitudin sit amet rhoncus sit amet, interdum vulputate mauris. Aliquam felis dui, cursus et gravida convallis, eleifend eget purus. Pellentesque non ultrices nisi. Curabitur eleifend ligula at mi elementum luctus. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Quisque viverra, orci nec pellentesque congue, lacus risus adipiscing tellus, eget tempor diam massa convallis justo. In iaculis nisl quis sem consectetur consectetur. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Morbi et magna nec justo cursus lobortis vel quis augue. Aenean nec eros et purus rutrum posuere. Mauris enim velit, lobortis a aliquam sed, consequat ut dolor. Nam vitae elit a quam tempor aliquet. Maecenas augue justo, laoreet ut commodo id, accumsan id ante.</p>

						<p>Vivamus convallis tempus iaculis. Quisque at eros nulla, et porta ligula. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi id lacus sit amet neque porta tincidunt ac non diam. Sed metus nibh, sollicitudin accumsan ornare vel, gravida et quam. Mauris ut leo nunc. Quisque sit amet lectus purus, in egestas massa. Ut sit amet erat quam, ac posuere leo.</p>

						<p>Vivamus arcu nulla, bibendum nec ullamcorper vitae, fringilla in nunc. Maecenas vel congue urna. Integer ligula dolor, laoreet non porta id, rhoncus a libero. Praesent hendrerit elementum sem quis pellentesque. Maecenas in nisi risus, a viverra massa. In ullamcorper dapibus massa non eleifend. Ut diam elit, commodo id lacinia vitae, fringilla at metus. Fusce luctus varius urna, eget dapibus eros aliquam ac. Praesent accumsan, est a lobortis rutrum, urna arcu vehicula mi, ac commodo nisi tortor et lectus. Phasellus faucibus justo quis nibh hendrerit a vestibulum urna lobortis. Morbi est turpis, gravida et porttitor et, vulputate non dolor. Proin a neque in leo ultrices condimentum. Praesent at mi odio, sit amet mattis erat.</p>

						<p>Nulla sed consequat tortor. Sed aliquet velit pellentesque felis iaculis quis tincidunt lectus accumsan. Proin at dapibus elit. Nunc vulputate, mi id lobortis volutpat, ante libero ornare sem, a porta erat ante a augue. Curabitur facilisis convallis nisi, vitae dictum lorem pellentesque sed. Sed vulputate molestie purus in venenatis. Sed a fermentum augue. Vestibulum tincidunt volutpat elit, quis porta est bibendum eu. Praesent est sem, malesuada ac lacinia vitae, cursus non tellus. Mauris fermentum enim nec nisl porta pulvinar. Proin justo nulla, consectetur ac lacinia et, condimentum eu ipsum.</p>
-->
					</div>
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