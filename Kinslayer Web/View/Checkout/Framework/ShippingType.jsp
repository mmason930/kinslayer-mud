<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
CartSaleProduct cartSaleProduct = (CartSaleProduct)request.getAttribute("CartSaleProduct");
%>
<select class="shippingType">
<%
Iterator<ShippingType> shippingTypeIter = ShippingType.getSetIterator();
while(shippingTypeIter.hasNext()) {
  ShippingType shippingType = shippingTypeIter.next();
%>
<option name="<%=shippingType.getStandardName()%>" value="<%=shippingType.value()%>" <%= (cartSaleProduct.getShippingType().equals(shippingType) ? "selected" : "") %>><%=shippingType.getStandardName()%> (<%=shippingType.getMinimumDays()%> - <%=shippingType.getMaximumDays()%> Business Days)</option>
<%
}
%>
</select>