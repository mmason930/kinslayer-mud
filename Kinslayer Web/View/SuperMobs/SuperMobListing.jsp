<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%

MobPrototype mobPrototype = (MobPrototype)request.getAttribute("MobPrototype");
Zone zone = (Zone)request.getAttribute("Zone");
KitWithItemsAndObjectPrototypes kitWithItemsAndObjectPrototypes = (KitWithItemsAndObjectPrototypes)request.getAttribute("KitWithItemsAndObjectPrototypes");
SuperMob superMob = (SuperMob)request.getAttribute("SuperMob");

List<SuperMob> superMobs = (List<SuperMob>)request.getAttribute("SuperMobs");
Map<Integer, MobPrototype> mobPrototypeMap = (Map<Integer, MobPrototype>)request.getAttribute("MobPrototypeMap");
%>
		<div id="pageContent" class="noTopMargin autoWidth">
		
<%
if(mobPrototype != null) {
%>
			<h1 id="pageTitle"><%=StringUtil.ConvertToHTML(StringUtil.properString(mobPrototype.getShortDescription()))%></h1>
			<div id="superMobLocation" class="italics">
<%
  if(zone != null) {
%>
			Located in the area known as <span class="bold"><%=StringUtil.ConvertToHTML(StringUtil.properString(zone.getName()))%></span>.
<%
  }
  else {
%>
            The whereabouts of this SuperMOB are unknown.
<%
  }
}
%>
			</div>
			<div id="contentInner">
				<div id="superMobLeftNav">
					<ul>
<%
for(SuperMob currentSuperMob : superMobs) {
  MobPrototype currentMobPrototype = mobPrototypeMap.get(currentSuperMob.getMobId());
  
  if(currentMobPrototype == null) {
    continue;
  }
%>
						<li>
							<a href="<%=WebSiteUrlUtil.getSuperMobListingUrl(webSupport.getInstanceDomain(), currentSuperMob.getMobId(), currentMobPrototype.getShortDescription()) %>" <%= (superMob != null && superMob.getId() == currentSuperMob.getId()) ? ("class='selected'") : ""%>><span><%=StringUtil.ConvertToHTML(StringUtil.properString(currentMobPrototype.getShortDescription()))%></span></a>
						</li>
<%
}
%>
				</div>
<%
if(mobPrototype != null) {
%>
				<div id="superMobRightNav">
					<div id="superMobDetailPanel">
						<img id="superMobImage" src="<%=superMob.getMobImageUrl() == null ? "./images/smobs/images/knight.jpg" : superMob.getMobImageUrl()%>"></img>
						<div id="superMobDescription">
						
							<%=StringUtil.ConvertToHTML(superMob.getDescription())%>
							<br/>
							<br/>							
							This SuperMOB is rated: <%=superMob.getDifficulty().getStandardName()%>
						</div>
<%
if(superMob.getMapImageUrl() != null) {
%>
						<img id="superMobMap" src="<%=superMob.getMapImageUrl()%>"></img>
<%
}
%>
					</div>
					
					<table id="superMobEquipmentTable">
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
          
          if(objectWearType.equals(ObjectWearType.take))
            continue;
          
          if(isFirst)
            isFirst = false;
          else
            objectWearDisplay += ", ";
          
          objectWearDisplay += StringUtil.ConvertToHTML(objectWearType.getDisplayTerm());
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
<%
}
%>
			</div>
		</div>