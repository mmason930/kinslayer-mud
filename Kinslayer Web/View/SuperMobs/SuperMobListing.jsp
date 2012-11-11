<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
	<tiles:insertTemplate template="/View/SuperMobs/Framework/SubNav.jsp" flush="true" />
	<div class="container_box">
	
		<div style="margin: 20px; float:left; width: 100%">
		
			<div>
			
<div class="smobList">
 <div class="classGroupingHeader">Choose a SuperMOB</div>
 <div class="smobListBody">
  <ul>
   <li>
    <a href="#"><span>Snake King</span></a>
   </li>
   <li>
    <a href="#"><span>Malandric</span></a>
   </li>
   <li>
    <a href="#"><span>Some SMOB</span></a>
   </li>
  </ul>
  <div style="clear:left;"></div>
 </div>
</div>

<div class="smobDetailsContainer">
  <div class="smobDetails">
   <span class="bold largeFont">Snake King</span><br/>
   <span class="italics mediumFont">Located in the area known as <span class="bold">Tar Valon</span>.</span>
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
    <tr>
     <td class="itemNameColumn">a worn golden signet ring</td>
     <td class="bodyLocationColumn">Finger</td>
     <td class="loadProbabilityColumn">95%</td>
    </tr>
   </table>


  </div>
</div>
			
			
			</div>
		</div>
		<div style="clear: both;"></div>
	</div> <!-- End of Container Box -->