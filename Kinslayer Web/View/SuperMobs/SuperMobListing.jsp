<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%

MobPrototype mobPrototype = (MobPrototype)request.getAttribute("MobPrototype");
Zone zone = (Zone)request.getAttribute("Zone");
List<MobPrototype> superMobPrototypes = (List<MobPrototype>)request.getAttribute("SuperMobPrototypes");
KitWithItemsAndObjectPrototypes kitWithItemsAndObjectPrototypes = (KitWithItemsAndObjectPrototypes)request.getAttribute("KitWithItemsAndObjectPrototypes");
%>
	<tiles:insertTemplate template="/View/SuperMobs/Framework/SubNav.jsp" flush="true" />
	<div class="container_box">
	
		<div style="margin: 20px; float:left; width: 100%">
			
			<div class="smobList">
				<div class="classGroupingHeader">Choose a SuperMOB</div>
				<div class="smobListBody">
					<ul>
<%
for(MobPrototype superMobPrototype : superMobPrototypes) {
%>
						<li>
							<a href="/supermob-listing?MobPrototypeId=<%=superMobPrototype.getId()%>"><span><%=StringUtil.ConvertToHTML(StringUtil.properString(superMobPrototype.getShortDescription()))%></span></a>
						</li>
<%
}
%>
					</ul>
					<div class="clearLeft"></div>
				</div>
			</div>
<%
if(mobPrototype != null) {
%>
			<div class="smobDetailsContainer">
				<div class="smobDetails">
					<span class="bold largeFont"><%=StringUtil.ConvertToHTML(StringUtil.properString(mobPrototype.getShortDescription())) %></span><br/>
					<span class="italics mediumFont">
<%
  if(zone != null) {
%>
						Located in the area known as <span class="bold"><%=StringUtil.ConvertToHTML(zone.getName())%></span>.
<%
  }
  else {
%>
                        The whereabouts of this superMOB are unknown.
<%
  }
%>
					</span>
				</div>
				<div class="smobEquipment">
					<table class="smobEquipmentTable">
						<tr>
							<th class="itemNameColumn">Item Name</th>
							<th class="bodyLocationColumn">Location</th>
							<th class="loadProbabilityColumn">Probability</th>
						</tr>
<%
  if(kitWithItemsAndObjectPrototypes != null) {
    Map<Integer, ObjectPrototype> idToObjectPrototypeMap = kitWithItemsAndObjectPrototypes.getIdToObjectPrototypeMap();
    
    for(List<KitItem> kitItemList : kitWithItemsAndObjectPrototypes.getKitWithItems().getKitItemLists()) {
    
      for(KitItem kitItem : kitItemList) {
        ObjectPrototype objectPrototype = idToObjectPrototypeMap.get(kitItem.getObjectPrototypeId());
        Set<ObjectWearType> objectWearTypes = objectPrototype.getObjectWearTypes();
        String objectWearDisplay = "";
        
        boolean isFirst = true;
        for(ObjectWearType objectWearType : objectWearTypes) {
          
          if(objectWearType.value() == ObjectWearType.take.value())
            continue;
          
          if(isFirst)
            isFirst = false;
          else
            objectWearDisplay += ", ";
          
          objectWearDisplay += ("<a href='#'>" + StringUtil.ConvertToHTML(objectWearType.getStandardName()) + "</a>");
        }
%>


						<tr>
							<td class="itemNameColumn"><%=StringUtil.ConvertToHTML(StringUtil.properString(objectPrototype.getShortDescription()))%></td>
							<td class="bodyLocationColumn"><%=objectWearDisplay%></td>
							<td class="loadProbabilityColumn"><%=kitItem.getProbability()%>%</td>
						</tr>
<%
      }
    }
  }
%>
					</table>
				</div>
			</div>
<%
}
%>
		</div>
		<div style="clear: both;"></div>
	</div> <!-- End of Container Box -->