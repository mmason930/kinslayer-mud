<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
List<ObjectPrototype> objectPrototypes = (List<ObjectPrototype>)request.getAttribute("ObjectPrototypes");
%>
					<div class="equipmentArmorTableContainer">

						<div class="itemNameColumn floatLeft">&nbsp;</div>
						<div class="dodgeColumn floatLeft">Dodge</div>
						<div class="parryColumn floatLeft">Parry</div>
						<div class="absColumn floatLeft">Absorb</div>
						<div class="weightColumn floatLeft">Weight</div>
						<div class="clearLeft"></div>

						<div class="equipmentArmorTableContainerInner">

							<table>
<%
for(ObjectPrototype objectPrototype : objectPrototypes) {
%>
								<tr>
									<td class="itemNameColumn"><%=StringUtil.properString(StringUtil.escapeHTMLCharacters(objectPrototype.getShortDescription())) %></td>
									<td class="dodgeColumn"><%=objectPrototype.getDodgeBonus() %></td>
									<td class="parryColumn"><%=objectPrototype.getParryBonus() %></td>
									<td class="absColumn"><%=objectPrototype.getAbsorb() %></td>
									<td class="weightColumn"><%=objectPrototype.getWeight() %></td>
								</tr>
<%
}
%>
							</table>
						</div>
					</div>