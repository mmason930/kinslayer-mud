package org.kinslayermud.util;

import java.math.BigDecimal;
import java.sql.Connection;
import java.sql.Statement;
import java.util.Collection;
import java.util.List;
import java.util.Map;

import org.kinslayermud.address.NameAddress;
import org.kinslayermud.cart.Cart;
import org.kinslayermud.cart.CartUtil;
import org.kinslayermud.cart.CartWithProducts;
import org.kinslayermud.exception.DataInterfaceException;
import org.kinslayermud.payment.PaymentMethodType;
import org.kinslayermud.paypal.PaypalAPIUtil;
import org.kinslayermud.paypal.PaypalGetExpressCheckoutDetailsResponse;
import org.kinslayermud.product.Product;
import org.kinslayermud.product.ProductType;
import org.kinslayermud.product.ProductUtil;
import org.kinslayermud.salelisting.SaleListing;
import org.kinslayermud.salelisting.SaleListingUtil;
import org.kinslayermud.search.ProductSearchResult;
import org.kinslayermud.search.ProductSearchUtil;
import org.kinslayermud.seller.Seller;
import org.kinslayermud.seller.SellerUtil;

public class WebSupportImp implements WebSupport {

  public void putProduct(Product product) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      
      ProductUtil.putProduct(statement, product);
      
      statement.close();
      connection.commit();
      connection.close();
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }

  public Product getProduct(String productCode) throws DataInterfaceException {
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      Product product;
      
      product = ProductUtil.getProduct(statement, productCode);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return product;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public List<ProductSearchResult> performProductSearch(ProductType productType, int offset, int fetchSize) throws DataInterfaceException {
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      List<ProductSearchResult> productSearchResults;
      
      productSearchResults = ProductSearchUtil.performProductSearch(statement, productType, offset, fetchSize);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return productSearchResults;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }

  public SaleListing getSaleListing(int saleListingId, boolean throwIfNotFound) throws DataInterfaceException {
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      SaleListing saleListing = null;
      
      saleListing = SaleListingUtil.getSaleListing(statement, saleListingId, throwIfNotFound, false);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return saleListing;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  

  public Product getProduct(int productId) throws DataInterfaceException {
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      Product product = null;
      
      product = ProductUtil.getProduct(statement, productId);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return product;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public Seller getSeller(int sellerUserId) throws DataInterfaceException {
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      Seller seller = null;
      
      seller = SellerUtil.getSeller(statement, sellerUserId, true);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return seller;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public Cart getCart(int cartId) throws DataInterfaceException {
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      Cart cart = null;
      
      cart = CartUtil.getCart(statement, cartId, false, false);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return cart;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  public void putCart(Cart cart) throws DataInterfaceException {
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      
      CartUtil.putCart(statement, cart);
      
      statement.close();
      connection.commit();
      connection.close();
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  public Cart createNewCart(String ipAddress) throws DataInterfaceException {
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      Cart cart = null;
      
      cart = CartUtil.createNewCart(statement, ipAddress);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return cart;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public void allocateSaleListingForCart(Cart cart, int saleListingId, short quantity) throws DataInterfaceException {
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      
      CartUtil.allocateSaleListingForCart(statement, cart, saleListingId, quantity);
      
      statement.close();
      connection.commit();
      connection.close();
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public CartWithProducts getCartWithProducts(int cartId) throws DataInterfaceException {
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      CartWithProducts cartWithProducts = null;
      
      cartWithProducts = CartUtil.getCartWithProducts(statement, cartId, true);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return cartWithProducts;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public Map<Integer, Product> getIdToProductMap(Collection<Integer> productIdCollection) throws DataInterfaceException {
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      Map<Integer, Product> idToProductMap = null;
      
      idToProductMap = ProductUtil.getIdToProductMap(statement, productIdCollection);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return idToProductMap;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public Map<Integer, Seller> getIdToSellerMap(Collection<Integer> sellerUserIdCollection) throws DataInterfaceException {
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      Map<Integer, Seller> idToSellerMap = null;
      
      idToSellerMap = SellerUtil.getSellerUserIdToSellerMap(statement, sellerUserIdCollection);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return idToSellerMap;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public String performSetExpressCheckout(int cartId, BigDecimal totalAmount) throws DataInterfaceException {
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      String token = null;
      
      token = PaypalAPIUtil.performSetExpressCheckout(totalAmount);
      
      Cart cart = CartUtil.getCart(statement, cartId, true, true);
      
      cart.setPaymentMethodType(PaymentMethodType.paypalExpress);
      cart.setPaymentToken(token);
      
      CartUtil.putCart(statement, cart);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return token;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }

  public void getExpressCheckoutDetails(int cartId, String token) throws DataInterfaceException {
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      
      PaypalGetExpressCheckoutDetailsResponse response = PaypalAPIUtil.getExpressCheckoutDetails(token);
      
      Cart cart = CartUtil.getCart(statement, cartId, true, true);
      
      cart.setShippingAddress(response.getShippingAddress());
      cart.setEmailAddress(response.getEmailAddress());
      cart.setPaymentMethodType(PaymentMethodType.paypalExpress);
      cart.setPaymentToken(token);
      cart.setPayerId(response.getPayerId());
      
      CartUtil.putCart(statement, cart);
      
      statement.close();
      connection.commit();
      connection.close();
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  public void completeCheckout(int cartId) throws DataInterfaceException {
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      
      CartUtil.completeCheckout(statement, cartId);
      
      statement.close();
      connection.commit();
      connection.close();
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  public void removeCartItem(int cartId, int cartSaleProductId) throws DataInterfaceException {
    Connection connection = null;
    Statement statement = null;
    try {
      connection = DatabaseUtil.getConnection();
      statement = connection.createStatement();
      
      CartUtil.removeCartItem(statement, cartId, cartSaleProductId);
      
      statement.close();
      connection.commit();
      connection.close();
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
}
