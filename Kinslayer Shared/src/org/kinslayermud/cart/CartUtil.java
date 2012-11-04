package org.kinslayermud.cart;

import java.math.BigDecimal;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Date;
import java.util.LinkedList;
import java.util.List;

import org.kinslayermud.address.NameAddress;
import org.kinslayermud.exception.NonExistentObjectException;
import org.kinslayermud.payment.PaymentMethodType;
import org.kinslayermud.paypal.PaypalAPIUtil;
import org.kinslayermud.paypal.PaypalDoExpressCheckoutPaymentResponse;
import org.kinslayermud.salelisting.SaleListing;
import org.kinslayermud.salelisting.SaleListingUtil;
import org.kinslayermud.saletransaction.SaleTransactionUtil;
import org.kinslayermud.saletransaction.SaleTransactionWithProducts;
import org.kinslayermud.shipping.ShippingType;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.QueryUtil;
import org.kinslayermud.util.SQLUtil;

public class CartUtil {

  public static Cart getCartMeetingCriteria(Statement statement, String criteria, boolean throwIfNotFound, boolean forUpdate) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT *"
               + " FROM cart"
               + " WHERE " + criteria
               + (forUpdate ? " FOR UPDATE" : "");
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getCart(resultSet);
    }
    
    if(throwIfNotFound) {
      
      throw new NonExistentObjectException("Cart not found. Criteria: " + criteria);
    }
    
    return null;
  }
  
  public static Cart getCart(Statement statement, int cartId, boolean throwIfNotFound, boolean forUpdate) throws SQLException, NonExistentObjectException {
    
    String criteria = " id = " + cartId;
    return getCartMeetingCriteria(statement, criteria, throwIfNotFound, forUpdate);
  }
  
  public static Cart getCart(ResultSet resultSet) throws SQLException {
    
    Cart cart = new Cart();

    cart.setId(resultSet.getInt("id"));
    cart.setIpAddress(resultSet.getString("ip_address"));
    cart.setRandomId(resultSet.getString("random_id"));
    cart.setCreatedDatetime(resultSet.getTimestamp("created_datetime"));
    cart.setEmailAddress(resultSet.getString("email_address"));
    cart.setLastModifiedDatetime(resultSet.getTimestamp("last_modified_datetime"));
    cart.setPayerId(resultSet.getString("payer_id"));
    cart.setPaymentToken(resultSet.getString("payment_token"));
    cart.setPaymentMethodType(PaymentMethodType.getEnum(resultSet.getInt("payment_method_type")));
    cart.setPhoneNumber(resultSet.getString("phone_number"));

    NameAddress billingAddress = new NameAddress();
    
    billingAddress.setName(resultSet.getString("billing_name"));
    billingAddress.setAddressLine1(resultSet.getString("billing_address_line_1"));
    billingAddress.setAddressLine2(resultSet.getString("billing_address_line_2"));
    billingAddress.setCity(resultSet.getString("billing_city"));
    billingAddress.setRegion(resultSet.getString("billing_state"));
    billingAddress.setPostalCode(resultSet.getString("billing_postal_code"));
    billingAddress.setCountry(resultSet.getString("billing_country"));
    cart.setBillingAddress(billingAddress);
    
    NameAddress shippingAddress = new NameAddress();
    
    shippingAddress.setName(resultSet.getString("shipping_name"));
    shippingAddress.setAddressLine1(resultSet.getString("shipping_address_line_1"));
    shippingAddress.setAddressLine2(resultSet.getString("shipping_address_line_2"));
    shippingAddress.setCity(resultSet.getString("shipping_city"));
    shippingAddress.setRegion(resultSet.getString("shipping_state"));
    shippingAddress.setPostalCode(resultSet.getString("shipping_postal_code"));
    shippingAddress.setCountry(resultSet.getString("shipping_country"));
    cart.setShippingAddress(shippingAddress);
    
    return cart;
  }
  
  public static void putCart(Statement statement, Cart cart) throws SQLException {
    
    String sql;
    
    NameAddress billingAddress = cart.getBillingAddress();
    NameAddress shippingAddress = cart.getShippingAddress();
    
    if(cart.isNew()) {
      
      sql = " INSERT INTO cart("
          + "   `ip_address`,"
          + "   `created_datetime`,"
          + "   `email_address`,"
          + "   `last_modified_datetime`,"
          + "   `payer_id`,"
          + "   `payment_token`,"
          + "   `payment_method_type`,"
          + "   `random_id`,"
          + "   `phone_number`,"
          + "   `billing_name`,"
          + "   `billing_address_line_1`,"
          + "   `billing_address_line_2`,"
          + "   `billing_city`,"
          + "   `billing_state`,"
          + "   `billing_postal_code`,"
          + "   `billing_country`,"
          + "   `shipping_name`,"
          + "   `shipping_address_line_1`,"
          + "   `shipping_address_line_2`,"
          + "   `shipping_city`,"
          + "   `shipping_state`,"
          + "   `shipping_postal_code`,"
          + "   `shipping_country`"
          + " ) VALUES("
          + SQLUtil.escapeQuoteString(cart.getIpAddress()) + ","
          + SQLUtil.encodeQuoteDate(cart.getCreatedDatetime()) + ","
          + SQLUtil.escapeQuoteString(cart.getEmailAddress()) + ","
          + SQLUtil.encodeQuoteDate(cart.getLastModifiedDatetime()) + ","
          + SQLUtil.escapeQuoteString(cart.getPayerId()) + ","
          + SQLUtil.escapeQuoteString(cart.getPaymentToken()) + ","
          + cart.getPaymentMethodType() + ","
          + SQLUtil.escapeQuoteString(cart.getRandomId()) + ","
          + SQLUtil.escapeQuoteString(cart.getPhoneNumber()) + ","
          + SQLUtil.escapeQuoteString(billingAddress.getName()) + ","
          + SQLUtil.escapeQuoteString(billingAddress.getAddressLine1()) + ","
          + SQLUtil.escapeQuoteString(billingAddress.getAddressLine2()) + ","
          + SQLUtil.escapeQuoteString(billingAddress.getCity()) + ","
          + SQLUtil.escapeQuoteString(billingAddress.getRegion()) + ","
          + SQLUtil.escapeQuoteString(billingAddress.getPostalCode()) + ","
          + SQLUtil.escapeQuoteString(billingAddress.getCountry()) + ","
          + SQLUtil.escapeQuoteString(shippingAddress.getName()) + ","
          + SQLUtil.escapeQuoteString(shippingAddress.getAddressLine1()) + ","
          + SQLUtil.escapeQuoteString(shippingAddress.getAddressLine2()) + ","
          + SQLUtil.escapeQuoteString(shippingAddress.getCity()) + ","
          + SQLUtil.escapeQuoteString(shippingAddress.getRegion()) + ","
          + SQLUtil.escapeQuoteString(shippingAddress.getPostalCode()) + ","
          + SQLUtil.escapeQuoteString(shippingAddress.getCountry()) + ")";
      
      statement.executeUpdate(sql);
      
      cart.setId(QueryUtil.getLastInsertedID(statement));
    }
    else {
      
      sql = " UPDATE cart SET"
          + "   ip_address =  " + SQLUtil.escapeQuoteString(cart.getIpAddress()) + ","
          + "   created_datetime = " + SQLUtil.encodeQuoteDate(cart.getCreatedDatetime()) + ","
          + "   email_address = " + SQLUtil.escapeQuoteString(cart.getEmailAddress()) + ","
          + "   last_modified_datetime = " + SQLUtil.encodeQuoteDate(cart.getLastModifiedDatetime()) + ","
          + "   payer_id = " + SQLUtil.escapeQuoteString(cart.getPayerId()) + ","
          + "   payment_token = " + SQLUtil.escapeQuoteString(cart.getPaymentToken()) + ","
          + "   payment_method_type = " + cart.getPaymentMethodType() + ","
          + "   random_id = " + SQLUtil.escapeQuoteString(cart.getRandomId()) + ","
          + "   phone_number = " + SQLUtil.escapeQuoteString(cart.getPhoneNumber()) + ","
          + "   billing_name = " + SQLUtil.escapeQuoteString(billingAddress.getName()) + ","
          + "   billing_address_line_1 = " + SQLUtil.escapeQuoteString(billingAddress.getAddressLine1()) + ","
          + "   billing_address_line_2 = " + SQLUtil.escapeQuoteString(billingAddress.getAddressLine2()) + ","
          + "   billing_city = " + SQLUtil.escapeQuoteString(billingAddress.getCity()) + ","
          + "   billing_state = " + SQLUtil.escapeQuoteString(billingAddress.getRegion()) + ","
          + "   billing_postal_code = " + SQLUtil.escapeQuoteString(billingAddress.getPostalCode()) + ","
          + "   billing_country = " + SQLUtil.escapeQuoteString(billingAddress.getCountry()) + ","
          + "   shipping_name = " + SQLUtil.escapeQuoteString(shippingAddress.getName()) + ","
          + "   shipping_address_line_1 = " + SQLUtil.escapeQuoteString(shippingAddress.getAddressLine1()) + ","
          + "   shipping_address_line_2 = " + SQLUtil.escapeQuoteString(shippingAddress.getAddressLine2()) + ","
          + "   shipping_city = " + SQLUtil.escapeQuoteString(shippingAddress.getCity()) + ","
          + "   shipping_state = " + SQLUtil.escapeQuoteString(shippingAddress.getRegion()) + ","
          + "   shipping_postal_code = " + SQLUtil.escapeQuoteString(shippingAddress.getPostalCode()) + ","
          + "   shipping_country = " + SQLUtil.escapeQuoteString(shippingAddress.getCountry())
          + " WHERE id = " + cart.getId();

      statement.executeUpdate(sql);
    }
  }
  
  public static CartSaleProduct getCartSaleProductMeetingCriteria(Statement statement, String criteria, boolean throwIfNotFound, boolean forUpdate) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT *"
               + " FROM cartSaleProduct"
               + " WHERE " + criteria
               + (forUpdate ? " FOR UPDATE" : "");
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getCartSaleProduct(resultSet);
    }
    
    if(throwIfNotFound) {
      
      throw new NonExistentObjectException("cartSaleProdct not found. Criteria: " + criteria);
    }
    
    return null;
  }
  
  public static List<CartSaleProduct> getCartSaleProductsMeetingCriteria(Statement statement, String criteria) throws SQLException {
    
    List<CartSaleProduct> cartSaleProducts = new LinkedList<CartSaleProduct>();
    String sql = " SELECT *"
               + " FROM cartSaleProduct"
               + " WHERE " + criteria;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      CartSaleProduct cartSaleProduct = getCartSaleProduct(resultSet);
      cartSaleProducts.add(cartSaleProduct);
    }
    
    return cartSaleProducts;
  }
  
  public static List<CartSaleProduct> getCartSaleProductsByCartId(Statement statement, int cartId) throws SQLException {
    
    String criteria = " cart_id = " + cartId;
    return getCartSaleProductsMeetingCriteria(statement, criteria);
  }
  
  public static CartSaleProduct getCartSaleProduct(Statement statement, int cartSaleProductId, boolean throwIfNotFound, boolean forUpdate) throws SQLException, NonExistentObjectException {
    
    String criteria=  " id = " + cartSaleProductId;
    return getCartSaleProductMeetingCriteria(statement, criteria, throwIfNotFound, forUpdate);
  }
  
  public static CartSaleProduct getCartSaleProduct(ResultSet resultSet) throws SQLException {
    
    CartSaleProduct cartSaleProduct = new CartSaleProduct();
    
    cartSaleProduct.setId(resultSet.getInt("id"));
    cartSaleProduct.setCartId(resultSet.getInt("cart_id"));
    cartSaleProduct.setSaleListingId(resultSet.getInt("sale_listing_id"));
    cartSaleProduct.setSellerUserId(resultSet.getInt("seller_user_id"));
    cartSaleProduct.setProductId(resultSet.getInt("product_id"));
    cartSaleProduct.setCreatedDatetime(resultSet.getTimestamp("created_datetime"));
    cartSaleProduct.setPrice(QueryUtil.getFixedIntBigDecimal(resultSet, "price"));
    cartSaleProduct.setShippingType(ShippingType.getEnum(resultSet.getInt("shipping_type")));
    cartSaleProduct.setShippingAmount(QueryUtil.getFixedIntBigDecimal(resultSet, "shipping_amount"));
    cartSaleProduct.setQuantity(resultSet.getShort("quantity"));
    cartSaleProduct.setCountyTaxRate(resultSet.getBigDecimal("county_tax_rate"));
    cartSaleProduct.setCountyTaxAmount(resultSet.getBigDecimal("county_tax_amount"));
    cartSaleProduct.setCityTaxRate(resultSet.getBigDecimal("city_tax_rate"));
    cartSaleProduct.setCityTaxAmount(resultSet.getBigDecimal("city_tax_amount"));
    cartSaleProduct.setStateTaxRate(resultSet.getBigDecimal("state_tax_rate"));
    cartSaleProduct.setStateTaxAmount(resultSet.getBigDecimal("state_tax_amount"));
    cartSaleProduct.setSpecialTaxRate(resultSet.getBigDecimal("special_tax_rate"));
    cartSaleProduct.setSpecialTaxAmount(resultSet.getBigDecimal("special_tax_amount"));
    
    return cartSaleProduct;
  }
  
  public static void putCartSaleProduct(Statement statement, CartSaleProduct cartSaleProduct) throws SQLException {
    
    String sql;
    
    if(cartSaleProduct.isNew()) {
      
      sql = " INSERT INTO cartSaleProduct("
          + "   `cart_id`,"
          + "   `sale_listing_id`,"
          + "   `seller_user_id`,"
          + "   `product_id`,"
          + "   `created_datetime`,"
          + "   `price`,"
          + "   `shipping_type`,"
          + "   `shipping_amount`,"
          + "   `quantity`,"
          + "   `county_tax_rate`,"
          + "   `county_tax_amount`,"
          + "   `city_tax_rate`,"
          + "   `city_tax_amount`,"
          + "   `state_tax_rate`,"
          + "   `state_tax_amount`,"
          + "   `special_tax_rate`,"
          + "   `special_tax_amount`"
          + " ) VALUES("
          + cartSaleProduct.getCartId() + ","
          + cartSaleProduct.getSaleListingId() + ","
          + cartSaleProduct.getSellerUserId() + ","
          + cartSaleProduct.getProductId() + ","
          + SQLUtil.encodeQuoteDate(cartSaleProduct.getCreatedDatetime()) + ","
          + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getPrice()) + ","
          + cartSaleProduct.getShippingType() + ","
          + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getShippingAmount()) + ","
          + cartSaleProduct.getQuantity() + ","
          + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getCountyTaxRate()) + ","
          + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getCountyTaxAmount()) + ","
          + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getCityTaxRate()) + ","
          + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getCityTaxAmount()) + ","
          + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getStateTaxRate()) + ","
          + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getStateTaxAmount()) + ","
          + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getSpecialTaxRate()) + ","
          + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getSpecialTaxAmount()) + ")";

      statement.executeUpdate(sql);
      
      cartSaleProduct.setId(QueryUtil.getLastInsertedID(statement));
    }
    else {
      
      sql = " UPDATE cartSaleProduct SET"
          + "   cart_id = " + cartSaleProduct.getCartId() + ","
          + "   sale_listing_id = " + cartSaleProduct.getSaleListingId() + ","
          + "   seller_user_id = " + cartSaleProduct.getSellerUserId() + ","
          + "   product_id = " + cartSaleProduct.getProductId() + ","
          + "   created_datetime = " + SQLUtil.encodeQuoteDate(cartSaleProduct.getCreatedDatetime()) + ","
          + "   price = " + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getPrice()) + ","
          + "   shipping_type = " + cartSaleProduct.getShippingType() + ","
          + "   shipping_amount = " + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getShippingAmount()) + ","
          + "   quantity = " + cartSaleProduct.getQuantity() + ","
          + "   county_tax_rate = " + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getCountyTaxRate()) + ","
          + "   county_tax_amount = " + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getCountyTaxAmount()) + ","
          + "   city_tax_rate = " + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getCityTaxRate()) + ","
          + "   city_tax_amount = " + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getCityTaxAmount()) + ","
          + "   state_tax_rate = " + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getStateTaxRate()) + ","
          + "   state_tax_amount = " + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getStateTaxAmount()) + ","
          + "   special_tax_rate = " + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getSpecialTaxRate()) + ","
          + "   special_tax_amount = " + SQLUtil.putFixedIntegerBigDecimal(cartSaleProduct.getSpecialTaxAmount())
          + " WHERE id = " + cartSaleProduct.getId();
      
      statement.executeUpdate(sql);
    }
  }
  
  public static void deleteCartSaleProduct(Statement statement, int cartSaleProductId) throws SQLException {
    
    String sql = " DELETE FROM cartSaleProduct"
               + " WHERE id = " + cartSaleProductId;
    
    statement.executeUpdate(sql);
  }
  
  public static CartWithProducts getCartWithProducts(Statement statement, int cartId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    CartWithProducts cartWithProducts = new CartWithProducts();
    List<CartSaleProduct> cartSaleProducts;
    Cart cart = getCart(statement, cartId, throwIfNotFound, false);
    
    if(cart == null) {
      
      return null;
    }
    
    cartSaleProducts = getCartSaleProductsByCartId(statement, cartId);
    cartWithProducts.setCartSaleProducts(cartSaleProducts);
    cartWithProducts.setCart(cart);
    
    return cartWithProducts;
  }
  
  public static Cart createNewCart(Statement statement, String ipAddress) throws SQLException {
    
    Cart cart = new Cart();

    cart.setCreatedDatetime(new Date());
    cart.setIpAddress(ipAddress);
    cart.setRandomId(MiscUtil.createRandomID(20));
    cart.setBillingAddress(new NameAddress());
    cart.setShippingAddress(new NameAddress());
    cart.setEmailAddress(null);
    cart.setPhoneNumber(null);
    cart.setLastModifiedDatetime(new Date());
    
    putCart(statement, cart);
    
    return cart;
  }
  
  public static void lockCart(Statement statement, int cartId) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT id"
               + " FROM cart"
               + " WHERE id= " + cartId
               + " FOR UPDATE";
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(!resultSet.next()) {
      
      throw new NonExistentObjectException("Could not lock cart #" + cartId + ": Cart not found.");
    }
  }
  
  public static void allocateSaleListingForCart(Statement statement, Cart cart, int saleListingId, short quantity) throws SQLException, NonExistentObjectException {
    
    lockCart(statement, cart.getId());
    
    SaleListingUtil.allocateSaleListing(statement, saleListingId, quantity);
    
    SaleListing saleListing = SaleListingUtil.getSaleListing(statement, saleListingId, true, false);
    
    CartSaleProduct cartSaleProduct = new CartSaleProduct();
    cartSaleProduct.setCartId(cart.getId());
    cartSaleProduct.setCreatedDatetime(new Date());
    cartSaleProduct.setPrice(saleListing.getPrice());
    cartSaleProduct.setQuantity(quantity);
    cartSaleProduct.setProductId(saleListing.getProductId());
    cartSaleProduct.setSaleListingId(saleListing.getId());
    cartSaleProduct.setSellerUserId(saleListing.getSellerUserId());
    cartSaleProduct.setCountyTaxRate(BigDecimal.ZERO);
    cartSaleProduct.setCountyTaxAmount(BigDecimal.ZERO);
    cartSaleProduct.setStateTaxRate(BigDecimal.ZERO);
    cartSaleProduct.setStateTaxAmount(BigDecimal.ZERO);
    cartSaleProduct.setCityTaxRate(BigDecimal.ZERO);
    cartSaleProduct.setCityTaxAmount(BigDecimal.ZERO);
    cartSaleProduct.setSpecialTaxRate(BigDecimal.ZERO);
    cartSaleProduct.setSpecialTaxAmount(BigDecimal.ZERO);
    cartSaleProduct.setShippingType(ShippingType.standard);
    cartSaleProduct.setShippingAmount(ShippingType.standard.getPrice());
    
    CartUtil.putCartSaleProduct(statement, cartSaleProduct);
    
    CartUtil.updateCartLastModifiedDatetime(statement, cart.getId());
  }
  
  public static void completeCheckout(Statement statement, int cartId) throws Exception {
    
    CartWithProducts cartWithProducts = CartUtil.getCartWithProducts(statement, cartId, true);
    
    System.out.println("Completing Paypal Express Transaction.");
    PaypalDoExpressCheckoutPaymentResponse response = PaypalAPIUtil.doExpressCheckoutPayment(cartWithProducts);
    
    System.out.println("Storing Paypal Express Transaction To Database...");
    int paymentId = PaypalAPIUtil.storeExpressCheckoutPayment(statement, response);
    
    System.out.println("Creating Sale Transaction With Products. Payment ID: " + paymentId);
    SaleTransactionWithProducts saleTransactionWithProducts = SaleTransactionUtil.createSaleTransactionWithProductsFromCart(statement, cartWithProducts, paymentId);
    
    System.out.println("Storing Sale Transaction With Products To Database...");
    SaleTransactionUtil.putSaleTransactionWithProducts(statement, saleTransactionWithProducts);
    
    System.out.println("Removing Inventory...");
    for(CartSaleProduct cartSaleProduct : cartWithProducts.getCartSaleProducts()) {
      
      SaleListingUtil.decreaseTotalCount(statement, cartSaleProduct.getSaleListingId(), cartSaleProduct.getQuantity());
    }
  }
  
  public static void removeCartItem(Statement statement, int cartId, int cartSaleProductId) throws Exception {
    
    lockCart(statement, cartId);
    
    CartSaleProduct cartSaleProduct = CartUtil.getCartSaleProduct(statement, cartSaleProductId, true, true);

    if(cartSaleProduct.getCartId() == cartId) {
      
      CartUtil.deleteCartSaleProduct(statement, cartSaleProductId);

      SaleListingUtil.deallocateSaleListing(statement, cartSaleProduct.getSaleListingId(), cartSaleProduct.getQuantity());
      
      CartUtil.updateCartLastModifiedDatetime(statement, cartId);
    }
  }
  
  public static void updateCartLastModifiedDatetime(Statement statement, int cartId) throws SQLException {
    
    String sql = " UPDATE cart SET"
               + "   last_modified_datetime = " + SQLUtil.encodeQuoteDate(new Date())
               + " WHERE id = " + cartId;
    
    statement.executeUpdate(sql);
  }
  
  public static List<Integer> getExpiredCartIdList(Statement statement, Date expirationDatetime) throws SQLException {
    
    String sql = " SELECT id"
               + " FROM cart"
               + " WHERE last_modified_datetime < " + SQLUtil.encodeQuoteDate(expirationDatetime);
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    List<Integer> cartIdList = new LinkedList<Integer>();
    
    while(resultSet.next()) {
      
      cartIdList.add(resultSet.getInt("id"));
    }
    
    return cartIdList;
  }
  
  public static void deleteCartAndProducts(Statement statement, int cartId) throws SQLException, NonExistentObjectException {
    
    lockCart(statement, cartId);
    
    List<CartSaleProduct> cartSaleProducts = getCartSaleProductsByCartId(statement, cartId);
    
    for(CartSaleProduct cartSaleProduct : cartSaleProducts) {
      
      CartUtil.deleteCartSaleProduct(statement, cartSaleProduct.getId());

      SaleListingUtil.deallocateSaleListing(statement, cartSaleProduct.getSaleListingId(), cartSaleProduct.getQuantity());  
    }
    
    CartUtil.deleteCartObject(statement, cartId);
  }
  
  public static void deleteCartObject(Statement statement, int cartId) throws SQLException {
    
    String sql = " DELETE FROM cart WHERE id = " + cartId;
    
    statement.executeUpdate(sql);
  }
}
