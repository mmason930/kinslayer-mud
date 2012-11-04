package org.kinslayermud.search;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.kinslayermud.product.Product;
import org.kinslayermud.product.ProductType;
import org.kinslayermud.product.ProductUtil;
import org.kinslayermud.salelisting.SaleListing;
import org.kinslayermud.salelisting.SaleListingUtil;
import org.kinslayermud.seller.Seller;
import org.kinslayermud.seller.SellerUtil;
import org.kinslayermud.util.SQLUtil;

public class ProductSearchUtil {

  public static List<ProductSearchResult> performProductSearch(Statement statement, ProductType productType, int offset, int fetchSize) throws SQLException {

    String sql;
    List<ProductSearchResult> productSearchResults = new LinkedList<ProductSearchResult>();
    Map<Integer, Product> idToProductMap = new HashMap<Integer, Product>();
    Set<Integer> sellerUserIdSet = new HashSet<Integer>();
    Map<Integer, Seller> sellerUserIdToSellerMap;
    Map<Integer, List<SaleListing>> productIdToSaleListingsMap = new HashMap<Integer, List<SaleListing>>();
    
    //Get the products.
    
    sql = " SELECT " + ProductUtil.PRODUCT_COLUMNS
        + " FROM product"
        + ProductUtil.PRODUCT_JOINS
        + " WHERE type = " + productType
        + " LIMIT " + offset + "," + fetchSize;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {

      Product product = ProductUtil.getProduct(resultSet);
      
      idToProductMap.put(product.getId(), product);
    }
    
    //Get the listings.
    sql = " SELECT *"
        + " FROM saleListing"
        + " WHERE product_id IN" + SQLUtil.buildListSQL(idToProductMap.keySet(), false, true)
        + " AND available_count > 0";
    
    resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      SaleListing saleListing = SaleListingUtil.getSaleListing(resultSet);
      
      sellerUserIdSet.add(saleListing.getSellerUserId());
      
      List<SaleListing> saleListings = productIdToSaleListingsMap.get(saleListing.getProductId());
      
      if(saleListings == null) {
        
        saleListings = new LinkedList<SaleListing>();
        
        productIdToSaleListingsMap.put(saleListing.getProductId(), saleListings);
      }
      
      saleListings.add(saleListing);
    }
 
    //Load the sellers.
    sellerUserIdToSellerMap = SellerUtil.getSellerUserIdToSellerMap(statement, sellerUserIdSet);
    
    for(int productId : productIdToSaleListingsMap.keySet()) {
      
      List<SaleListing> saleListings = productIdToSaleListingsMap.get(productId);
      ProductSearchResult productSearchResult = new ProductSearchResult();
      
      productSearchResult.setSaleListings(saleListings);
      productSearchResult.setProduct(idToProductMap.get(productId));
      productSearchResult.setIdToSellerMap(sellerUserIdToSellerMap); //TODO: This map actually has more than is needed.
      
      productSearchResults.add(productSearchResult);
    }
    
    return productSearchResults;
  }
}
