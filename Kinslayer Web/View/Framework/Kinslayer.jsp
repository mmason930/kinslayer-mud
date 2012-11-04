<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ page import="org.kinslayermud.product.*" %>
<%@ page import="org.kinslayermud.util.*" %>
<%@ page import="org.kinslayermud.salelisting.*" %>
<%@ page import="org.kinslayermud.seller.*" %>
<%@ page import="org.kinslayermud.user.*" %>
<%@ page import="org.kinslayermud.search.*" %>
<%@ page import="org.kinslayermud.cart.*" %>
<%@ page import="org.kinslayermud.image.*" %>
<%@ page import="org.kinslayermud.shipping.*" %>
<%@ page import="java.util.*" %>
<%!

String getSearchResultsListing(ProductType productType) {

  if(productType.equals(ProductType.grocery)) {
    return "/View/Search/Framework/SearchResults/SearchResultsGroceryListing.jsp";
  }
  if(productType.equals(ProductType.book)) {
    return "/View/Search/Framework/SearchResults/SearchResultsBookListing.jsp";
  }

  return null;
}

String getCartSaleProductDisplay(ProductType productType) {

  if(productType.equals(ProductType.grocery)) {
    return "/View/Checkout/Framework/CartSaleProductDisplay/CartSaleProductDisplayGrocery.jsp";
  }
  if(productType.equals(ProductType.book)) {
    return "/View/Checkout/Framework/CartSaleProductDisplay/CartSaleProductDisplayBook.jsp";
  }
  
  return null;
}

%>