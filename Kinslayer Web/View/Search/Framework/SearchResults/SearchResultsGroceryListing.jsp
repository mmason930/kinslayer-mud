<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
ProductSearchResult productSearchResult = (ProductSearchResult)request.getAttribute("ProductSearchResult");
Product product = productSearchResult.getProduct();
SaleListing saleListing = productSearchResult.getLowestPricedListing();
Grocery grocery = (Grocery)product;

String saleListingUrl = WebSiteUrlUtil.getSaleListingUrl(saleListing.getId());

%>
Name: <a href="<%=saleListingUrl%>"><%=product.getName()%></a><br/>
Price: <%=MiscUtil.formatMoneyValue(saleListing.getPrice())%><br/>