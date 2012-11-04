package org.kinslayermud.seller;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.kinslayermud.address.NameAddress;
import org.kinslayermud.exception.NonExistentObjectException;
import org.kinslayermud.user.UserUtil;
import org.kinslayermud.util.QueryUtil;
import org.kinslayermud.util.SQLUtil;

public class SellerUtil {

  public static Seller getSellerMeetingCriteria(Statement statement, String criteria, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT *"
               + " FROM seller, user"
               + " WHERE seller.user_id = user.id"
               + " AND " + criteria;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getSeller(resultSet);
    }
    
    if(throwIfNotFound) {
      
      throw new NonExistentObjectException("Seller not found. Criteria: " + criteria);
    }
    
    return null;
  }
  
  public static Seller getSeller(Statement statement, int sellerUserId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String criteria = " user_id = " + sellerUserId;
    return getSellerMeetingCriteria(statement, criteria, throwIfNotFound);
  }
  
  public static Seller getSeller(ResultSet resultSet) throws SQLException {
    
    Seller seller = new Seller();
    
    UserUtil.fillInBaseFields(resultSet, seller);
    
    seller.setUsername(resultSet.getString("username"));
    seller.setStatus(SellerStatus.getEnum(resultSet.getInt("status")));
    seller.setStatusLastModifiedDatetime(resultSet.getTimestamp("status_last_modified_datetime"));
    seller.setCommissionRate(resultSet.getBigDecimal("commission_rate"));
    seller.setLastLoginDatetime(resultSet.getTimestamp("last_login_datetime"));
    seller.setTotalOrders(resultSet.getInt("total_orders"));
    seller.setTotalOrderItems(resultSet.getInt("total_order_items"));
    seller.setTotalShippedItems(resultSet.getInt("total_shipped_items"));
    seller.setTotalCancelledItems(resultSet.getInt("total_cancelled_items"));
    
    return seller;
  }
  
  public static void putSeller(Statement statement, Seller seller) throws SQLException {
    
    String sql;
    
    boolean isNew = seller.isNew();
    
    UserUtil.putUser(statement, seller);
    
    if(!isNew) {
      
      sql = " INSERT INTO seller("
          + "   `user_id`,"
          + "   `username`,"
          + "   `status`,"
          + "   `status_last_modified_datetime`,"
          + "   `commission_rate`,"
          + "   `last_login_datetime`,"
          + "   `total_orders`,"
          + "   `total_order_items`,"
          + "   `total_shipped_items`,"
          + "   `total_cancelled_items`"
          + " ) VALUES("
          + seller.getId() + ","
          + SQLUtil.escapeQuoteString(seller.getUsername()) + ","
          + seller.getStatus() + ","
          + SQLUtil.encodeQuoteDate(seller.getStatusLastModifiedDatetime()) + ","
          + seller.getCommissionRate() + ","
          + SQLUtil.encodeQuoteDate(seller.getLastLoginDatetime()) + ","
          + seller.getTotalOrders() + ","
          + seller.getTotalOrderItems() + ","
          + seller.getTotalShippedItems() + ","
          + seller.getTotalCancelledItems() + ")";
      
      statement.executeUpdate(sql);
      
      seller.setId(QueryUtil.getLastInsertedID(statement));
    }
    else {
      
      sql = " UPDATE seller SET"
          + "   username = " + SQLUtil.escapeQuoteString(seller.getUsername()) + ","
          + "   status = " + seller.getStatus() + ","
          + "   status_last_modified_datetime = " + SQLUtil.encodeQuoteDate(seller.getStatusLastModifiedDatetime()) + ","
          + "   commission_rate = " + seller.getCommissionRate() + ","
          + "   last_login_datetime = " + SQLUtil.encodeQuoteDate(seller.getLastLoginDatetime()) + ","
          + "   total_orders = " + seller.getTotalOrders() + ","
          + "   total_order_items = " + seller.getTotalOrderItems() + ","
          + "   total_shipped_items = " + seller.getTotalShippedItems() + ","
          + "   total_cancelled_items = " + seller.getTotalCancelledItems()
          + " WHERE user_id = " + seller.getId();
      
      statement.executeUpdate(sql);
    }
  }
  
  public static Map<Integer, Seller> getSellerUserIdToSellerMap(Statement statement, Collection<Integer> userIdCollection) throws SQLException {
    
    Map<Integer, Seller> sellerUserIdToSellerMap = new HashMap<Integer, Seller>();
    String sql = " SELECT *"
               + " FROM seller, user"
               + " WHERE seller.user_id = user.id"
               + " AND seller.user_id IN" + SQLUtil.buildListSQL(userIdCollection, false, true);
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      Seller seller = getSeller(resultSet);
      sellerUserIdToSellerMap.put(seller.getId(), seller);
    }
    
    return sellerUserIdToSellerMap;
  }
  
  public static SellerCompanyAddress getSellerCompanyAddressMeetingCriteria(Statement statement, String criteria, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT *"
               + " FROM sellerCompanyAddress"
               + " WHERE criteria";
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getSellerCompanyAddress(resultSet);
    }
    
    if(throwIfNotFound) {
      
      throw new NonExistentObjectException("No sellerCompanyAddress found. Criteria: " + criteria);
    }
    
    return null;
  }
  
  public static SellerCompanyAddress getSellerCompanyAddress(Statement statement, int sellerId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String criteria = " seller_id = " + sellerId;
    return getSellerCompanyAddressMeetingCriteria(statement, criteria, throwIfNotFound);
  }
  
  public static SellerCompanyAddress getSellerCompanyAddress(ResultSet resultSet) throws SQLException {
    
    SellerCompanyAddress sellerCompanyAddress = new SellerCompanyAddress();

    sellerCompanyAddress.setId(resultSet.getInt("id"));
    sellerCompanyAddress.setSellerId(resultSet.getInt("seller_id"));
    
    NameAddress address = new NameAddress();
    address.setName(resultSet.getString("name"));
    address.setAddressLine1(resultSet.getString("address_line_1"));
    address.setAddressLine2(resultSet.getString("address_line_2"));
    address.setCity(resultSet.getString("city"));
    address.setRegion(resultSet.getString("region"));
    address.setPostalCode(resultSet.getString("postal_code"));
    address.setCountry(resultSet.getString("country"));
    
    sellerCompanyAddress.setAddress(address);
    
    return sellerCompanyAddress;
  }
  
  public static void putSellerCompanyAddress(Statement statement, SellerCompanyAddress sellerCompanyAddress) throws SQLException {
    
    String sql;
    NameAddress address = sellerCompanyAddress.getAddress();
    
    if(sellerCompanyAddress.isNew()) {
      
      sql = " INSERT INTO sellerCompanyAddress("
          + "   `seller_id`,"
          + "   `name`,"
          + "   `address_line_1`,"
          + "   `address_line_2`,"
          + "   `city`,"
          + "   `region`,"
          + "   `postal_code`,"
          + "   `country`"
          + " ) VALUES("
          + sellerCompanyAddress.getSellerId() + ","
          + SQLUtil.escapeQuoteString(address.getName()) + ","
          + SQLUtil.escapeQuoteString(address.getAddressLine1()) + ","
          + SQLUtil.escapeQuoteString(address.getAddressLine2()) + ","
          + SQLUtil.escapeQuoteString(address.getCity()) + ","
          + SQLUtil.escapeQuoteString(address.getRegion()) + ","
          + SQLUtil.escapeQuoteString(address.getPostalCode()) + ","
          + SQLUtil.escapeQuoteString(address.getCountry()) + ")";
      
      statement.executeQuery(sql);
      
      sellerCompanyAddress.setId(QueryUtil.getLastInsertedID(statement));
    }
    else {
      
      sql = " UPDATE sellerCompanyAddress SET"
          + "   seller_id = " + sellerCompanyAddress.getSellerId() + ","
          + "   name = " + SQLUtil.escapeQuoteString(address.getName()) + ","
          + "   address_line_1 = " + SQLUtil.escapeQuoteString(address.getAddressLine1()) + ","
          + "   address_line_2 = " + SQLUtil.escapeQuoteString(address.getAddressLine2()) + ","
          + "   city = " + SQLUtil.escapeQuoteString(address.getCity()) + ","
          + "   region = " + SQLUtil.escapeQuoteString(address.getRegion()) + ","
          + "   postal_code = " + SQLUtil.escapeQuoteString(address.getPostalCode()) + ","
          + "   country = " + SQLUtil.escapeQuoteString(address.getCountry())
          + " WHERE id = " + sellerCompanyAddress.getId();
      
      statement.executeQuery(sql);
    }
  }
  
  public static SellerPaymentAddress getSellerPaymentAddressMeetingCriteria(Statement statement, String criteria, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT *"
               + " FROM sellerPaymentAddress"
               + " WHERE criteria";
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getSellerPaymentAddress(resultSet);
    }
    
    if(throwIfNotFound) {
      
      throw new NonExistentObjectException("No sellerPaymentAddress found. Criteria: " + criteria);
    }
    
    return null;
  }
  
  public static SellerPaymentAddress getSellerPaymentAddress(Statement statement, int sellerId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String criteria = " seller_id = " + sellerId;
    return getSellerPaymentAddressMeetingCriteria(statement, criteria, throwIfNotFound);
  }
  
  public static SellerPaymentAddress getSellerPaymentAddress(ResultSet resultSet) throws SQLException {
    
    SellerPaymentAddress sellerPaymentAddress = new SellerPaymentAddress();

    sellerPaymentAddress.setId(resultSet.getInt("id"));
    sellerPaymentAddress.setSellerId(resultSet.getInt("seller_id"));
    
    NameAddress address = new NameAddress();
    address.setName(resultSet.getString("name"));
    address.setAddressLine1(resultSet.getString("address_line_1"));
    address.setAddressLine2(resultSet.getString("address_line_2"));
    address.setCity(resultSet.getString("city"));
    address.setRegion(resultSet.getString("region"));
    address.setPostalCode(resultSet.getString("postal_code"));
    address.setCountry(resultSet.getString("country"));
    
    sellerPaymentAddress.setAddress(address);
    
    return sellerPaymentAddress;
  }
  
  public static void putSellerPaymentAddress(Statement statement, SellerPaymentAddress sellerPaymentAddress) throws SQLException {
    
    String sql;
    NameAddress address = sellerPaymentAddress.getAddress();
    
    if(sellerPaymentAddress.isNew()) {
      
      sql = " INSERT INTO sellerPaymentAddress("
          + "   `seller_id`,"
          + "   `name`,"
          + "   `address_line_1`,"
          + "   `address_line_2`,"
          + "   `city`,"
          + "   `region`,"
          + "   `postal_code`,"
          + "   `country`"
          + " ) VALUES("
          + sellerPaymentAddress.getSellerId() + ","
          + SQLUtil.escapeQuoteString(address.getName()) + ","
          + SQLUtil.escapeQuoteString(address.getAddressLine1()) + ","
          + SQLUtil.escapeQuoteString(address.getAddressLine2()) + ","
          + SQLUtil.escapeQuoteString(address.getCity()) + ","
          + SQLUtil.escapeQuoteString(address.getRegion()) + ","
          + SQLUtil.escapeQuoteString(address.getPostalCode()) + ","
          + SQLUtil.escapeQuoteString(address.getCountry()) + ")";
      
      statement.executeQuery(sql);
      
      sellerPaymentAddress.setId(QueryUtil.getLastInsertedID(statement));
    }
    else {
      
      sql = " UPDATE sellerPaymentAddress SET"
          + "   seller_id = " + sellerPaymentAddress.getSellerId() + ","
          + "   name = " + SQLUtil.escapeQuoteString(address.getName()) + ","
          + "   address_line_1 = " + SQLUtil.escapeQuoteString(address.getAddressLine1()) + ","
          + "   address_line_2 = " + SQLUtil.escapeQuoteString(address.getAddressLine2()) + ","
          + "   city = " + SQLUtil.escapeQuoteString(address.getCity()) + ","
          + "   region = " + SQLUtil.escapeQuoteString(address.getRegion()) + ","
          + "   postal_code = " + SQLUtil.escapeQuoteString(address.getPostalCode()) + ","
          + "   country = " + SQLUtil.escapeQuoteString(address.getCountry())
          + " WHERE id = " + sellerPaymentAddress.getId();
      
      statement.executeQuery(sql);
    }
  }
}
