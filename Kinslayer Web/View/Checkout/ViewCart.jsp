<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
CartWithProducts cartWithProducts = (CartWithProducts)request.getAttribute("CartWithProducts");
Cart cart = cartWithProducts.getCart();

Map<Integer, Product> idToProductMap = (Map<Integer, Product>)request.getAttribute("IdToProductMap");
Map<Integer, Seller> idToSellerMap = (Map<Integer, Seller>)request.getAttribute("IdToSellerMap");
%>

<script type="text/javascript">
$(document).ready(function() {

	$(".shippingType").bind("change", function(event) {

		var shippingTypeValue = $(event.target).val();

		$.ajax(
			{
				type: "POST",
				url: "/SetItemShipping",
				data: { "ShippingType" : shippingTypeValue },
				cache: false
			}
		).done(function( msg )
			{
				var jsonObject = JSON.parse(msg);
			}
		);
	});
});
</script>
Ohai, welcum 2 ur fkn cart<br/><br/>
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
<br/><br/>
Cart Total: <%=MiscUtil.formatMoneyValue(cartWithProducts.getTotalAmount())%><br/><br/>
<a href="/PaypalExpressCheckout">
 <img src="https://www.paypal.com/en_US/i/btn/btn_xpressCheckout.gif" align="left" style="margin-right:7px;">
</a>