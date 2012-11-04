<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
Product product = (Product)request.getAttribute("Product");
Seller seller = (Seller)request.getAttribute("Seller");
CartSaleProduct cartSaleProduct = (CartSaleProduct)request.getAttribute("CartSaleProduct");
String imageUrl = WebSiteUrlUtil.getImageUrl(ImageSize.full, product.getProductCode());
%>

<img src="<%=imageUrl%>" />
Name: <%=product.getName() %><br/>
UPC: <%=product.getProductCode() %><br/>
Seller: <%=seller.getUsername() %><br/>
Price: <%=MiscUtil.formatMoneyValue(cartSaleProduct.getPrice()) %><br/>
Quantity: <%=cartSaleProduct.getQuantity()%><br/>
Shipping: <tiles:insertTemplate template="/View/Checkout/Framework/ShippingType.jsp"/>
<a href="/RemoveFromCart?CartSaleProductId=<%=cartSaleProduct.getId()%>">Remove From Cart</a>
<br/>