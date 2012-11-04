package org.kinslayermud.salelisting;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import org.kinslayermud.exception.NonExistentObjectException;
import org.kinslayermud.util.QueryUtil;
import org.kinslayermud.util.SQLUtil;

public class SaleListingUtil {

  public static SaleListing getSaleListingMeetingCriteria(Statement statement, String criteria, boolean throwIfNotFound, boolean forUpdate) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT *"
               + " FROM saleListing"
               + " WHERE " + criteria
               + (forUpdate ? " FOR UPDATE" : "");
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getSaleListing(resultSet);
    }
    
    if(throwIfNotFound) {
      
      throw new NonExistentObjectException("saleListing not found. Criteria: " + criteria);
    }
    
    return null;
  }
  
  public static SaleListing getSaleListing(Statement statement, int saleListingId, boolean throwIfNotFound, boolean forUpdate) throws SQLException, NonExistentObjectException {
    
    String criteria = " id = " + saleListingId;
    return getSaleListingMeetingCriteria(statement, criteria, throwIfNotFound, forUpdate);
  }
  
  public static SaleListing getSaleListing(ResultSet resultSet) throws SQLException {
    
    SaleListing saleListing = new SaleListing();
    
    saleListing.setId(resultSet.getInt("id"));
    saleListing.setProductId(resultSet.getInt("product_id"));
    saleListing.setSellerUserId(resultSet.getInt("seller_user_id"));
    saleListing.setPrice(QueryUtil.getFixedIntBigDecimal(resultSet, "price"));
    saleListing.setSku(resultSet.getString("sku"));
    saleListing.setTotalCount(resultSet.getShort("total_count"));
    saleListing.setAvailableCount(resultSet.getShort("available_count"));
    
    return saleListing;
  }
  
  public static void putSaleListing(Statement statement, SaleListing saleListing) throws SQLException {
    
    String sql;
    
    if(saleListing.isNew()) {
      
      sql = " INSERT INTO saleListing("
          + "   `product_id`,"
          + "   `seller_user_id`,"
          + "   `price`,"
          + "   `sku`,"
          + "   `total_count`,"
          + "   `available_count`"
          + " ) VALUES("
          + saleListing.getProductId() + ","
          + saleListing.getSellerUserId() + ","
          + SQLUtil.putFixedIntegerBigDecimal(saleListing.getPrice()) + ","
          + SQLUtil.escapeQuoteString(saleListing.getSku()) + ","
          + saleListing.getTotalCount() + ","
          + saleListing.getAvailableCount() + ")";
      
      statement.executeQuery(sql);
      
      saleListing.setId(QueryUtil.getLastInsertedID(statement));
    }
    else {
      
      sql = " UPDATE saleListing SET"
          + "   product_id = " + saleListing.getProductId() + ","
          + "   seller_user_id = " + saleListing.getSellerUserId() + ","
          + "   price = " + SQLUtil.putFixedIntegerBigDecimal(saleListing.getPrice()) + ","
          + "   sku = " + SQLUtil.escapeQuoteString(saleListing.getSku()) + ","
          + "   total_count = " + saleListing.getTotalCount() + ","
          + "   available_count = " + saleListing.getAvailableCount()
          + " WHERE id = " + saleListing.getId();
      
      statement.executeUpdate(sql);
    }
  }
  
  public static void allocateSaleListing(Statement statement, int saleListingId, short quantity) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT available_count"
               + " FROM saleListing"
               + " WHERE id = " + saleListingId
               + " AND available_count >= " + quantity
               + " FOR UPDATE";
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(!resultSet.next()) {
      
      throw new NonExistentObjectException("saleLiting #" + saleListingId + " with quantity `" + quantity + "` not found.");
    }
    
    sql = " UPDATE saleListing SET"
        + "   available_count = available_count - " + quantity
        + " WHERE id = " + saleListingId;
    
    statement.executeUpdate(sql);
  }
  
  public static void deallocateSaleListing(Statement statement, int saleListingId, int quantity) throws SQLException, NonExistentObjectException {
   
    String sql = " UPDATE saleListing SET"
               + "   available_count = available_count + " + quantity
               + " WHERE id = " + saleListingId;
    
    statement.executeUpdate(sql);
  }
  
  public static void decreaseTotalCount(Statement statement, int saleListingId, int decreaseAmount) throws SQLException {
    
    String sql = " UPDATE saleListing SET"
               + "   total_count = total_count - " + decreaseAmount
               + " WHERE id = " + saleListingId;
    
    statement.executeUpdate(sql);
  }
}
