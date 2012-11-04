package org.kinslayermud.web.checkout;

import java.util.Map;

import org.kinslayermud.cart.Cart;
import org.kinslayermud.cart.CartWithProducts;
import org.kinslayermud.product.Product;
import org.kinslayermud.seller.Seller;
import org.kinslayermud.web.actions.StoreAction;

public class ViewCartAction extends StoreAction {

  protected static final String SUCCESS_FORWARD = "Success";
  protected static final String FAILURE_FORWARD = "Failure";
  
  public String execute(Cart cart) throws Exception {

    CartWithProducts cartWithProducts;
    Map<Integer, Product> idToProductMap;
    Map<Integer, Seller> idToSellerMap;
    
    cartWithProducts = webSupport.getCartWithProducts(cart.getId());
    idToProductMap = webSupport.getIdToProductMap(cartWithProducts.getProductIdSet());
    idToSellerMap = webSupport.getIdToSellerMap(cartWithProducts.getSellerUserIdSet());

    request.setAttribute("CartWithProducts", cartWithProducts);
    request.setAttribute("IdToProductMap", idToProductMap);
    request.setAttribute("IdToSellerMap", idToSellerMap);
    
    return SUCCESS_FORWARD;
  }
}
