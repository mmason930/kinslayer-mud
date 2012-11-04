<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<tiles:useAttribute name="quantity" classname="java.lang.Short" />
<tiles:useAttribute name="parameterName" classname="java.lang.String" />
<select name="<%=parameterName%>">
<%
if(quantity > 99) {
  quantity = (short)99;
}
for(short quantityCount = 1;quantityCount <= quantity;++quantityCount) {
%>
 <option value="<%=quantityCount%>" <%= (quantityCount == 1 ? "selected " : "") %> ><%=quantityCount%></option>
<%
}
%>
</select>