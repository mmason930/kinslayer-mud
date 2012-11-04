package org.kinslayermud.product;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import org.kinslayermud.util.SQLUtil;


public class ProductUtil {

  public static final String PRODUCT_JOINS = " LEFT JOIN book ON book.product_id = product.id"
                                              + " LEFT JOIN grocery ON grocery.product_id = product.id";
  public static final String PRODUCT_COLUMNS = " product.*, book.*, grocery.*";

  public static ProductOperator getProductOperator(ProductType productType) {
    
    if(productType == ProductType.book)
      return new BookOperator();
    if(productType == ProductType.grocery)
      return new GroceryOperator();
    return null;
  }
  
  public static Product getProduct(ResultSet resultSet) throws SQLException {
    
    ProductType productType = ProductType.getEnum(resultSet.getInt("product.type"));
    ProductOperator productOperator = getProductOperator(productType);
    return productOperator.getProduct(resultSet);
  }
  
  public static void putProduct(Statement statement, Product product) throws SQLException {
    
    ProductOperator productOperator = getProductOperator(product.getType());
    productOperator.putProduct(statement, product);
  }
  
  public static Product getProductMatchingCriteria(Statement statement, String criteria) throws SQLException {
    
    String sql = " SELECT " + PRODUCT_COLUMNS
               + " FROM product " + PRODUCT_JOINS
               + " WHERE " + criteria;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      return getProduct(resultSet);
    }
    
    return null;
  }
  
  public static Map<Integer, Product> getIdToProductMap(Statement statement, Collection<Integer> productIdCollection) throws SQLException {
    
    Map<Integer, Product> idToProductMap = new HashMap<Integer, Product>();
    String sql = " SELECT " + PRODUCT_COLUMNS
               + " FROM product " + PRODUCT_JOINS
               + " WHERE product.id IN" + SQLUtil.buildListSQL(productIdCollection, false, true);
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      Product product = getProduct(resultSet);
      
      idToProductMap.put(product.getId(), product);
    }
    
    return idToProductMap;
  }
  
  public static Product getProduct(Statement statement, String productCode) throws SQLException {
    
    String criteria = "product.product_code = " + productCode;
    return getProductMatchingCriteria(statement, criteria);
  }
  
  public static Product getProduct(Statement statement, int productId) throws SQLException {
    
    String criteria = "product.id = " + productId;
    return getProductMatchingCriteria(statement, criteria);
  }
}
