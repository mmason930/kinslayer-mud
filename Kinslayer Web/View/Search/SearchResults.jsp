<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
List<ProductSearchResult> productSearchResults = (List<ProductSearchResult>)request.getAttribute("ProductSearchResults");
System.out.println("Product Search Results: " + productSearchResults.size());
%>
<%
for(ProductSearchResult productSearchResult : productSearchResults) {
  Product product = productSearchResult.getProduct();
  request.setAttribute("ProductSearchResult", productSearchResult);
%>
<tiles:insertTemplate template="<%=getSearchResultsListing(product.getType())%>" />
<%
}
%>