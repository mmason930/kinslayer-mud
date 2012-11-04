<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
CartWithProducts cartWithProducts = (CartWithProducts)request.getAttribute("CartWithProducts");
Map<Integer, Product> idToProductMap = (Map<Integer, Product>)request.getAttribute("IdToProductMap");
Map<Integer, Seller> idToSellerMap = (Map<Integer, Seller>)request.getAttribute("IdToSellerMap");

Cart cart = cartWithProducts.getCart();
%>

Review ur fkn order<br/><br/>
<%

for(CartSaleProduct cartSaleProduct : cartWithProducts.getCartSaleProducts()) {

  Product product = idToProductMap.get(cartSaleProduct.getProductId());
  Seller seller = idToSellerMap.get(cartSaleProduct.getSellerUserId());

  request.setAttribute("Product", product);
  request.setAttribute("Seller", seller);
  request.setAttribute("CartSaleProduct", cartSaleProduct);
%>

<tiles:insertTemplate template="<%=getCartSaleProductDisplay(product.getType())%>" />

<%
}

%>
<br/>
<a href="/CompleteCheckout">Complete Checkout</a>