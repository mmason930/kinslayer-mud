<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
List<ObjectPrototype> objectPrototypes = (List<ObjectPrototype>)request.getAttribute("ObjectPrototypes");
%>
					<div class="equipmentWeaponTableContainer">
						
						<div class="itemNameColumn floatLeft">&nbsp;</div>
						<div class="offensiveColumn floatLeft">Offensive</div>
						<div class="parryColumn floatLeft">Parry</div>
						<div class="weightColumn floatLeft">Weight</div>
						<div class="damageLow floatLeft">Low Damage</div>
						<div class="damageHigh floatLeft">High Damage</div>
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
									<td class="damageLow"><%=objectPrototype.getValue(1) %></td>
									<td class="damageHigh"><%=objectPrototype.getValue(2) %></td>
								</tr>
<%
}
%>
							</table>
						</div>
					</div>