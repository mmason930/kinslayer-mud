<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
  SaleListing saleListing = (SaleListing)request.getAttribute("SaleListing");
  Product product = (Product)request.getAttribute("Product");
  Seller seller = (Seller)request.getAttribute("Seller");
%>

<form method="post" action="/AddCartSaleProduct">
Product Name: <%=product.getName() %><br/>
Seller Name: <%=seller.getUsername() %><br/>
Listing Price: <%=MiscUtil.formatMoneyValue(saleListing.getPrice()) %><br/>
Quantity:
 <tiles:insertTemplate template="/View/Framework/QuantityDropdown.jsp">
  <tiles:putAttribute name="quantity" value="<%=saleListing.getAvailableCount()%>" />
  <tiles:putAttribute name="parameterName" value="Quantity" />
 </tiles:insertTemplate>

 <input type="hidden" name="SaleListingId" value="<%=saleListing.getId()%>" />
 <input type="Submit" value="Add To Cart" />
</form><br/>
