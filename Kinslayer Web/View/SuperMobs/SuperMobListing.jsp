<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%

MobPrototype mobPrototype = (MobPrototype)request.getAttribute("MobPrototype");
Zone zone = (Zone)request.getAttribute("Zone");
List<MobPrototype> superMobPrototypes = (List<MobPrototype>)request.getAttribute("SuperMobPrototypes");
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
							<a href="#"><span><%=StringUtil.ConvertToHTML(superMobPrototype.getShortDescription())%></span></a>
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
					<span class="bold largeFont"><%=StringUtil.ConvertToHTML(mobPrototype.getShortDescription()) %></span><br/>
					<span class="italics mediumFont">
<%
  if(zone != null) {
%>
						Located in the area known as <span class="bold">Tar Valon</span>.
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
							<th class="bodyLocationColumn">Body Location</th>
							<th class="loadProbabilityColumn">Load Probability</th>
						</tr>
						<tr>
							<td class="itemNameColumn">a worn golden signet ring</td>
							<td class="bodyLocationColumn">Finger</td>
							<td class="loadProbabilityColumn">95%</td>
						</tr>
					</table>
				</div>
			</div>
<%
}
%>
		</div>
		<div style="clear: both;"></div>
	</div> <!-- End of Container Box -->