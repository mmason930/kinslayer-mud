package org.kinslayermud.util;

import java.math.BigDecimal;
import java.util.Collection;
import java.util.List;
import java.util.Map;

import org.kinslayermud.cart.Cart;
import org.kinslayermud.cart.CartWithProducts;
import org.kinslayermud.exception.DataInterfaceException;
import org.kinslayermud.product.Product;
import org.kinslayermud.product.ProductType;
import org.kinslayermud.salelisting.SaleListing;
import org.kinslayermud.search.ProductSearchResult;
import org.kinslayermud.seller.Seller;

public interface WebSupport {

  public Product getProduct(String productCode) throws DataInterfaceException;
  public Product getProduct(int productId) throws DataInterfaceException;
  public Seller getSeller(int sellerUserId) throws DataInterfaceException;
  public void putProduct(Product product) throws DataInterfaceException;
  public List<ProductSearchResult> performProductSearch(ProductType productType, int offset, int fetchSize) throws DataInterfaceException;
  public SaleListing getSaleListing(int saleListingId, boolean throwIfNotFound) throws DataInterfaceException;
  public Cart getCart(int cartId) throws DataInterfaceException;
  public void putCart(Cart cart) throws DataInterfaceException;
  public Cart createNewCart(String ipAddress) throws DataInterfaceException;
  public void allocateSaleListingForCart(Cart cart, int saleListing, short quantity) throws DataInterfaceException;
  public CartWithProducts getCartWithProducts(int cartId) throws DataInterfaceException;
  public Map<Integer, Product> getIdToProductMap(Collection<Integer> productIdCollection) throws DataInterfaceException;
  public Map<Integer, Seller> getIdToSellerMap(Collection<Integer> sellerUserIdCollection) throws DataInterfaceException;
  public String performSetExpressCheckout(int cartId, BigDecimal bigDecimal) throws DataInterfaceException;
  public void getExpressCheckoutDetails(int cartId, String token) throws DataInterfaceException;
  public void completeCheckout(int cartId) throws DataInterfaceException;
  public void removeCartItem(int cartId, int cartSaleProductId) throws DataInterfaceException;
}
