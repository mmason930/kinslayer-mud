package org.kinslayermud.saletransaction;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.kinslayermud.address.NameAddress;
import org.kinslayermud.cart.Cart;
import org.kinslayermud.cart.CartSaleProduct;
import org.kinslayermud.cart.CartWithProducts;
import org.kinslayermud.exception.NonExistentObjectException;
import org.kinslayermud.payment.PaymentMethodType;
import org.kinslayermud.seller.Seller;
import org.kinslayermud.seller.SellerUtil;
import org.kinslayermud.shipping.Carrier;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.QueryUtil;
import org.kinslayermud.util.SQLUtil;

public class SaleTransactionUtil {

  public static SaleTransaction getSaleTransactionMeetingCriteria(Statement statement, String criteria, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT *"
               + " FROM saleTransaction"
               + " WHERE " + criteria;
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getSaleTransaction(resultSet);
    }
    
    if(throwIfNotFound) {
      
      throw new NonExistentObjectException("saleTransaction not found. Criteria: " + criteria);
    }
    
    return null;
  }
  
  public static SaleTransaction getSaleTransaction(Statement statement, int saleTransactionId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String criteria = " id = " + saleTransactionId;
    return getSaleTransactionMeetingCriteria(statement, criteria, throwIfNotFound);
  }
  
  public static SaleTransaction getSaleTransaction(ResultSet resultSet) throws SQLException {
    
    SaleTransaction saleTransaction = new SaleTransaction();
    
    saleTransaction.setId(resultSet.getInt("id"));
    saleTransaction.setEmailAddress(resultSet.getString("email_address"));
    saleTransaction.setIpAddress(resultSet.getString("ip_address"));
    saleTransaction.setCreatedDatetime(resultSet.getTimestamp("created_datetime"));
    saleTransaction.setStatus(SaleTransactionStatus.getEnum(resultSet.getInt("status")));
    saleTransaction.setStatusLastModifiedDatetime(resultSet.getTimestamp("status_last_modified_datetime"));
    saleTransaction.setAutoCancelDatetime(resultSet.getTimestamp("auto_cancel_datetime"));
    saleTransaction.setPhoneNumber(resultSet.getString("phone_number"));
    saleTransaction.setRandomId(resultSet.getString("random_id"));
    saleTransaction.setName(resultSet.getString("name"));
    saleTransaction.setPaymentMethodType(PaymentMethodType.getEnum(resultSet.getInt("payment_method_type")));
    saleTransaction.setPaymentId(resultSet.getInt("payment_id"));
    
    NameAddress shippingAddress = new NameAddress();
    shippingAddress.setName(resultSet.getString("shipping_name"));
    shippingAddress.setAddressLine1(resultSet.getString("shipping_address_line_1"));
    shippingAddress.setAddressLine2(resultSet.getString("shipping_address_line_2"));
    shippingAddress.setCity(resultSet.getString("shipping_city"));
    shippingAddress.setRegion(resultSet.getString("shipping_region"));
    shippingAddress.setPostalCode(resultSet.getString("shipping_postal_code"));
    shippingAddress.setCountry(resultSet.getString("shipping_country"));
    
    saleTransaction.setShippingAddress(shippingAddress);
    
    return saleTransaction;
  }
  
  public static void putSaleTransaction(Statement statement, SaleTransaction saleTransaction) throws SQLException {
    
    String sql;
    NameAddress shippingAddress = saleTransaction.getShippingAddress();
    
    if(saleTransaction.isNew()) {
      
      
      sql = " INSERT INTO saleTransaction("
          + "   `email_address`,"
          + "   `ip_address`,"
          + "   `created_datetime`,"
          + "   `status`,"
          + "   `status_last_modified_datetime`,"
          + "   `auto_cancel_datetime`,"
          + "   `phone_number`,"
          + "   `random_id`,"
          + "   `name`,"
          + "   `payment_method_type`,"
          + "   `payment_id`,"
          + "   `shipping_name`,"
          + "   `shipping_address_line_1`,"
          + "   `shipping_address_line_2`,"
          + "   `shipping_city`,"
          + "   `shipping_region`,"
          + "   `shipping_postal_code`,"
          + "   `shipping_country`"
          + " ) VALUES("
          + SQLUtil.escapeQuoteString(saleTransaction.getEmailAddress()) + ","
          + SQLUtil.escapeQuoteString(saleTransaction.getIpAddress()) + ","
          + SQLUtil.encodeQuoteDate(saleTransaction.getCreatedDatetime()) + ","
          + saleTransaction.getStatus() + ","
          + SQLUtil.encodeQuoteDate(saleTransaction.getStatusLastModifiedDatetime()) + ","
          + SQLUtil.encodeQuoteDate(saleTransaction.getAutoCancelDatetime()) + ","
          + SQLUtil.escapeQuoteString(saleTransaction.getPhoneNumber()) + ","
          + SQLUtil.escapeQuoteString(saleTransaction.getRandomId()) + ","
          + SQLUtil.escapeQuoteString(saleTransaction.getName()) + ","
          + saleTransaction.getPaymentMethodType() + ","
          + saleTransaction.getPaymentId() + ","
          + SQLUtil.escapeQuoteString(shippingAddress.getName()) + ","
          + SQLUtil.escapeQuoteString(shippingAddress.getAddressLine1()) + ","
          + SQLUtil.escapeQuoteString(shippingAddress.getAddressLine2()) + ","
          + SQLUtil.escapeQuoteString(shippingAddress.getCity()) + ","
          + SQLUtil.escapeQuoteString(shippingAddress.getRegion()) + ","
          + SQLUtil.escapeQuoteString(shippingAddress.getPostalCode()) + ","
          + SQLUtil.escapeQuoteString(shippingAddress.getCountry()) + ")";

      statement.executeUpdate(sql);
      
      saleTransaction.setId(QueryUtil.getLastInsertedID(statement));
    }
    else {
      
      sql = " UPDATE saleTransaction SET"
          + "   email_address = " + SQLUtil.escapeQuoteString(saleTransaction.getEmailAddress()) + ","
          + "   ip_address = " + SQLUtil.escapeQuoteString(saleTransaction.getIpAddress()) + ","
          + "   created_datetime = " + SQLUtil.encodeQuoteDate(saleTransaction.getCreatedDatetime()) + ","
          + "   status = " + saleTransaction.getStatus() + ","
          + "   status_last_modified_datetime = " + SQLUtil.encodeQuoteDate(saleTransaction.getStatusLastModifiedDatetime()) + ","
          + "   auto_cancel_datetime = " + SQLUtil.encodeQuoteDate(saleTransaction.getAutoCancelDatetime()) + ","
          + "   phone_number = " + SQLUtil.escapeQuoteString(saleTransaction.getPhoneNumber()) + ","
          + "   random_id = " + SQLUtil.escapeQuoteString(saleTransaction.getRandomId()) + ","
          + "   name = " + SQLUtil.escapeQuoteString(saleTransaction.getName()) + ","
          + "   payment_method_type = " + saleTransaction.getPaymentMethodType() + ","
          + "   payment_id = " + saleTransaction.getPaymentId() + ","
          + "   shipping_name = " + SQLUtil.escapeQuoteString(shippingAddress.getName()) + ","
          + "   shipping_address_line_1 = " + SQLUtil.escapeQuoteString(shippingAddress.getAddressLine1()) + ","
          + "   shipping_address_line_2 = " + SQLUtil.escapeQuoteString(shippingAddress.getAddressLine2()) + ","
          + "   shipping_city = " + SQLUtil.escapeQuoteString(shippingAddress.getCity()) + ","
          + "   shipping_region = " + SQLUtil.escapeQuoteString(shippingAddress.getRegion()) + ","
          + "   shipping_postal_code = " + SQLUtil.escapeQuoteString(shippingAddress.getPostalCode()) + ","
          + "   shipping_country = " + SQLUtil.escapeQuoteString(shippingAddress.getCountry())
          + " WHERE id = " + saleTransaction.getId();
      
      statement.executeUpdate(sql);
    }
  }
  
  public static SaleTransactionProduct getSaleTransactionProductMeetingCriteria(Statement statement, String criteria, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT *"
               + " FROM saleTransaction"
               + " WHERE " + criteria;
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getSaleTransactionProduct(resultSet);
    }
    
    if(throwIfNotFound) {
      
      throw new NonExistentObjectException("saleTransactionProduct not found. Criteria: " + criteria);
    }
    
    return null;
  }
  
  public static SaleTransactionProduct getSaleTransactionProduct(Statement statement, int saleTransactionProductId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String criteria = " id = " + saleTransactionProductId;
    return getSaleTransactionProductMeetingCriteria(statement, criteria, throwIfNotFound);
  }
  
  public static SaleTransactionProduct getSaleTransactionProduct(ResultSet resultSet) throws SQLException {
    
    SaleTransactionProduct saleTransactionProduct = new SaleTransactionProduct();
  
    saleTransactionProduct.setId(resultSet.getInt("id"));
    saleTransactionProduct.setSaleTransactionId(resultSet.getInt("sale_transaction_id"));
    saleTransactionProduct.setProductId(resultSet.getInt("product_id"));
    saleTransactionProduct.setSellerUserId(resultSet.getInt("seller_user_id"));
    saleTransactionProduct.setInventoryId(resultSet.getInt("inventory_id"));
    saleTransactionProduct.setStatus(SaleTransactionProductStatus.getEnum(resultSet.getInt("status")));
    saleTransactionProduct.setStatusLastModifiedDatetime(resultSet.getTimestamp("status_last_modified_datetime"));
    saleTransactionProduct.setItemAmount(QueryUtil.getFixedIntBigDecimal(resultSet, "item_amount"));
    saleTransactionProduct.setShippingAmount(QueryUtil.getFixedIntBigDecimal(resultSet, "shipping_amount"));
    saleTransactionProduct.setCommissionRate(resultSet.getBigDecimal("commission_rate"));
    saleTransactionProduct.setItemCommissionAmount(QueryUtil.getFixedIntBigDecimal(resultSet, "item_commission_amount"));
    saleTransactionProduct.setShippingCommissionAmount(QueryUtil.getFixedIntBigDecimal(resultSet, "shipping_commission_amount"));
    
    saleTransactionProduct.setCountyTaxRate(resultSet.getBigDecimal("county_tax_rate"));
    saleTransactionProduct.setCountyTaxAmount(QueryUtil.getFixedIntBigDecimal(resultSet, "county_tax_amount"));
    saleTransactionProduct.setCityTaxRate(resultSet.getBigDecimal("city_tax_rate"));
    saleTransactionProduct.setCityTaxAmount(QueryUtil.getFixedIntBigDecimal(resultSet, "city_tax_amount"));
    saleTransactionProduct.setStateTaxRate(resultSet.getBigDecimal("state_tax_rate"));
    saleTransactionProduct.setStateTaxAmount(QueryUtil.getFixedIntBigDecimal(resultSet, "state_tax_amount"));
    saleTransactionProduct.setSpecialTaxRate(resultSet.getBigDecimal("special_tax_rate"));
    saleTransactionProduct.setSpecialTaxAmount(QueryUtil.getFixedIntBigDecimal(resultSet, "special_tax_amount"));

    Integer carrierId = QueryUtil.getInteger(resultSet, "carrier_id");
    
    if(carrierId != null) {
      saleTransactionProduct.setCarrier(Carrier.getEnum(carrierId));
    }
    
    saleTransactionProduct.setTrackingNumber(resultSet.getString("tracking_number"));
    saleTransactionProduct.setTrackingUploadedDatetime(resultSet.getTimestamp("tracking_uploaded_datetime"));
    
    return saleTransactionProduct;
  }
  
  public static void putSaleTransactionProduct(Statement statement, SaleTransactionProduct saleTransactionProduct) throws SQLException {
    
    String sql;
    
    if(saleTransactionProduct.isNew()) {
      
      sql = " INSERT INTO saleTransactionProduct("
          + "   `sale_transaction_id`,"
          + "   `product_id`,"
          + "   `seller_user_id`,"
          + "   `inventory_id`,"
          + "   `status`,"
          + "   `status_last_modified_datetime`,"
          + "   `item_amount`,"
          + "   `shipping_amount`,"
          + "   `commission_rate`,"
          + "   `item_commission_amount`,"
          + "   `shipping_commission_amount`,"
          + "   `county_tax_rate`,"
          + "   `county_tax_amount`,"
          + "   `city_tax_rate`,"
          + "   `city_tax_amount`,"
          + "   `state_tax_rate`,"
          + "   `state_tax_amount`,"
          + "   `special_tax_rate`,"
          + "   `special_tax_amount`,"
          + "   `carrier_id`,"
          + "   `tracking_number`,"
          + "   `tracking_uploaded_datetime`"
          + " ) VALUES("
          + saleTransactionProduct.getSaleTransactionId() + ","
          + saleTransactionProduct.getProductId() + ","
          + saleTransactionProduct.getSellerUserId() + ","
          + saleTransactionProduct.getInventoryId() + ","
          + saleTransactionProduct.getStatus() + ","
          + SQLUtil.encodeQuoteDate(saleTransactionProduct.getStatusLastModifiedDatetime()) + ","
          + SQLUtil.putFixedIntegerBigDecimal(saleTransactionProduct.getItemAmount()) + ","
          + SQLUtil.putFixedIntegerBigDecimal(saleTransactionProduct.getShippingAmount()) + ","
          + saleTransactionProduct.getCommissionRate() + ","
          + SQLUtil.putFixedIntegerBigDecimal(saleTransactionProduct.getItemCommissionAmount()) + ","
          + SQLUtil.putFixedIntegerBigDecimal(saleTransactionProduct.getShippingCommissionAmount()) + ","
          + saleTransactionProduct.getCountyTaxRate() + ","
          + SQLUtil.putFixedIntegerBigDecimal(saleTransactionProduct.getCountyTaxAmount()) + ","
          + saleTransactionProduct.getCityTaxRate() + ","
          + SQLUtil.putFixedIntegerBigDecimal(saleTransactionProduct.getCityTaxAmount()) + ","
          + saleTransactionProduct.getStateTaxRate() + ","
          + SQLUtil.putFixedIntegerBigDecimal(saleTransactionProduct.getStateTaxAmount()) + ","
          + saleTransactionProduct.getSpecialTaxRate() + ","
          + SQLUtil.putFixedIntegerBigDecimal(saleTransactionProduct.getSpecialTaxAmount()) + ","
          + saleTransactionProduct.getCarrier() + ","
          + SQLUtil.escapeQuoteString(saleTransactionProduct.getTrackingNumber()) + ","
          + SQLUtil.encodeQuoteDate(saleTransactionProduct.getTrackingUploadedDatetime()) + ")";

      statement.executeUpdate(sql);
      
      saleTransactionProduct.setId(QueryUtil.getLastInsertedID(statement));
    }
    else {
      
      sql = " UPDATE saleTransactionProduct SET"
          + "   sale_transaction_id = " + saleTransactionProduct.getSaleTransactionId() + ","
          + "   product_id = " + saleTransactionProduct.getProductId() + ","
          + "   seller_user_id = " + saleTransactionProduct.getSellerUserId() + ","
          + "   inventory_id = " + saleTransactionProduct.getInventoryId() + ","
          + "   status = " + saleTransactionProduct.getStatus() + ","
          + "   status_last_modified_datetime = " + SQLUtil.encodeQuoteDate(saleTransactionProduct.getStatusLastModifiedDatetime()) + ","
          + "   item_amount = " + SQLUtil.putFixedIntegerBigDecimal(saleTransactionProduct.getItemAmount()) + ","
          + "   shipping_amount = " + SQLUtil.putFixedIntegerBigDecimal(saleTransactionProduct.getShippingAmount()) + ","
          + "   commission_rate = " + saleTransactionProduct.getCommissionRate() + ","
          + "   item_commission_amount = " + SQLUtil.putFixedIntegerBigDecimal(saleTransactionProduct.getItemCommissionAmount()) + ","
          + "   shipping_commission_amount = " + SQLUtil.putFixedIntegerBigDecimal(saleTransactionProduct.getShippingCommissionAmount()) + ","
          + "   county_tax_rate = " + saleTransactionProduct.getCountyTaxRate() + ","
          + "   county_tax_amount = " + SQLUtil.putFixedIntegerBigDecimal(saleTransactionProduct.getCountyTaxAmount()) + ","
          + "   city_tax_rate = " + saleTransactionProduct.getCityTaxRate() + ","
          + "   city_tax_amount = " + SQLUtil.putFixedIntegerBigDecimal(saleTransactionProduct.getCityTaxAmount()) + ","
          + "   state_tax_rate = " + saleTransactionProduct.getStateTaxRate() + ","
          + "   state_tax_amount = " + SQLUtil.putFixedIntegerBigDecimal(saleTransactionProduct.getStateTaxAmount()) + ","
          + "   special_tax_rate = " + saleTransactionProduct.getSpecialTaxRate() + ","
          + "   special_tax_amount = " + SQLUtil.putFixedIntegerBigDecimal(saleTransactionProduct.getSpecialTaxAmount()) + ","
          + "   carrier_id = " + saleTransactionProduct.getCarrier() + ","
          + "   tracking_number = " + SQLUtil.escapeQuoteString(saleTransactionProduct.getTrackingNumber()) + ","
          + "   tracking_uploaded_datetime = " + SQLUtil.encodeQuoteDate(saleTransactionProduct.getTrackingUploadedDatetime())
          + " WHERE id = " + saleTransactionProduct.getId();
      
      statement.executeUpdate(sql);
    }
  }
  
  public static Date calculateAutoCancelDatetime(Date orderCreatedDatetime) {
    
    int days = 3;
    Calendar calendar = new GregorianCalendar();
    calendar.setTime(orderCreatedDatetime);
    
    while(days > 0) {
      
      calendar.add(Calendar.DATE, 1);
      
      if(calendar.get(Calendar.DAY_OF_WEEK) != Calendar.SATURDAY && calendar.get(Calendar.DAY_OF_WEEK) != Calendar.SUNDAY) {
        
        --days;
      }
    }
    
    return calendar.getTime();
  }
  
  public static SaleTransactionWithProducts createSaleTransactionWithProductsFromCart(Statement statement, CartWithProducts cartWithProducts, int paymentId) throws SQLException {
    
    SaleTransactionWithProducts saleTransactionWithProducts = new SaleTransactionWithProducts();
    SaleTransaction saleTransaction = new SaleTransaction();
    List<SaleTransactionProduct> saleTransactionProducts = new LinkedList<SaleTransactionProduct>();
    Map<Integer, Seller> idToSellerMap;

    Cart cart = cartWithProducts.getCart();
    List<CartSaleProduct> cartSaleProducts = cartWithProducts.getCartSaleProducts();
    Date createdDatetime = new Date();
    Date autoCancelDatetime = calculateAutoCancelDatetime(createdDatetime);

    saleTransactionWithProducts.setSaleTransaction(saleTransaction);
    saleTransactionWithProducts.setSaleTransactionProducts(saleTransactionProducts);

    saleTransaction.setEmailAddress(cart.getEmailAddress());
    saleTransaction.setIpAddress(cart.getIpAddress());
    saleTransaction.setCreatedDatetime(new Date());
    saleTransaction.setName(cart.getShippingAddress().getName());
    saleTransaction.setStatus(SaleTransactionStatus.complete);
    saleTransaction.setStatusLastModifiedDatetime(createdDatetime);
    saleTransaction.setAutoCancelDatetime(autoCancelDatetime);
    saleTransaction.setPhoneNumber(cart.getPhoneNumber());
    saleTransaction.setRandomId(MiscUtil.createRandomID(40));
    saleTransaction.setPaymentMethodType(cart.getPaymentMethodType());
    saleTransaction.setPaymentId(paymentId);
    saleTransaction.setShippingAddress(NameAddress.clone(cart.getShippingAddress()));
    
    idToSellerMap = SellerUtil.getSellerUserIdToSellerMap(statement, cartWithProducts.getSellerUserIdSet());

    for(CartSaleProduct cartSaleProduct : cartSaleProducts) {
      
      int quantityRemaining = cartSaleProduct.getQuantity();
      
      while(quantityRemaining > 0) {

        SaleTransactionProduct saleTransactionProduct = new SaleTransactionProduct();
        Seller seller = idToSellerMap.get(cartSaleProduct.getSellerUserId());

        saleTransactionProduct.setProductId(cartSaleProduct.getProductId());
        saleTransactionProduct.setSellerUserId(cartSaleProduct.getSellerUserId());
        saleTransactionProduct.setInventoryId(cartSaleProduct.getSaleListingId());
        saleTransactionProduct.setStatus(SaleTransactionProductStatus.pending);
        saleTransactionProduct.setStatusLastModifiedDatetime(createdDatetime);
        saleTransactionProduct.setItemAmount(cartSaleProduct.getPrice());
        saleTransactionProduct.setShippingAmount(cartSaleProduct.getShippingAmount());

        saleTransactionProduct.setCommissionRate(seller.getCommissionRate());
        saleTransactionProduct.setItemCommissionAmount(cartSaleProduct.getPrice().multiply(saleTransactionProduct.getCommissionRate().movePointLeft(2)));
        saleTransactionProduct.setShippingCommissionAmount(cartSaleProduct.getShippingAmount().multiply(saleTransactionProduct.getCommissionRate().movePointLeft(2)));

        saleTransactionProduct.setCountyTaxRate(cartSaleProduct.getCountyTaxRate());
        saleTransactionProduct.setCountyTaxAmount(cartSaleProduct.getCountyTaxAmount());
        saleTransactionProduct.setCityTaxRate(cartSaleProduct.getCityTaxRate());
        saleTransactionProduct.setCityTaxAmount(cartSaleProduct.getCityTaxAmount());
        saleTransactionProduct.setStateTaxRate(cartSaleProduct.getStateTaxRate());
        saleTransactionProduct.setStateTaxAmount(cartSaleProduct.getStateTaxAmount());
        saleTransactionProduct.setSpecialTaxRate(cartSaleProduct.getSpecialTaxRate());
        saleTransactionProduct.setSpecialTaxAmount(cartSaleProduct.getSpecialTaxAmount());

        saleTransactionProducts.add(saleTransactionProduct);
        
        --quantityRemaining;
      }
    }

    return saleTransactionWithProducts;
  }
  
  public static void putSaleTransactionWithProducts(Statement statement, SaleTransactionWithProducts saleTransactionWithProducts) throws SQLException {
    
    SaleTransaction saleTransaction = saleTransactionWithProducts.getSaleTransaction();
    
    boolean isNew = saleTransaction.isNew();
    
    putSaleTransaction(statement, saleTransaction);
    
    for(SaleTransactionProduct saleTransactionProduct : saleTransactionWithProducts.getSaleTransactionProducts()) {
      
      if(isNew) {
        
        saleTransactionProduct.setSaleTransactionId(saleTransaction.getId());
      }
      
      putSaleTransactionProduct(statement, saleTransactionProduct);
    }
  }
}
