<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
List<ObjectPrototype> objectPrototypes = (List<ObjectPrototype>)request.getAttribute("ObjectPrototypes");
%>				
				<div class="equipmentRightPanel">
	
					<div class="equipmentWeaponTableContainer">

						<div class="itemNameColumn floatLeft">&nbsp;</div>
						<div class="offensiveColumn floatLeft">Offensive</div>
						<div class="parryColumn floatLeft">Parry</div>
						<div class="weightColumn floatLeft">Weight</div>
						<div class="clearLeft"></div>

						<div class="equipmentWeaponTableContainerInner">

							<table>
<%
for(ObjectPrototype objectPrototype : objectPrototypes) {
%>
								<tr>
									<td class="itemNameColumn"><%=StringUtil.properString(StringUtil.escapeHTMLCharacters(objectPrototype.getShortDescription())) %></td>
									<td class="offensiveColumn"><%=objectPrototype.getOffensiveBonus() %></td>
									<td class="parryColumn"><%=objectPrototype.getParryBonus() %></td>
									<td class="weightColumn"><%=objectPrototype.getWeight() %></td>
								</tr>
<%
}
%>
							</table>
						</div>
					</div>
				</div>