package org.kinslayermud.web.checkout;

import java.util.Map;

import org.kinslayermud.cart.Cart;
import org.kinslayermud.cart.CartWithProducts;
import org.kinslayermud.product.Product;
import org.kinslayermud.seller.Seller;
import org.kinslayermud.web.actions.StoreAction;

public class ReviewOrderAction extends StoreAction {

  protected static final String SUCCESS_FORWARD = "Success";
  protected static final String FAILURE_FORWARD = "Failure";
  protected static final String SHIPPING_OR_PAYMENT_NOT_COMPLETE_FORWARD = "ShippingOrPaymentNotComplete";
  
  public String execute(Cart cart) throws Exception {
    
    try {
      
      if(cart.getShippingAddress() == null) {
        
        return SHIPPING_OR_PAYMENT_NOT_COMPLETE_FORWARD;
      }
      
      CartWithProducts cartWithProducts = webSupport.getCartWithProducts(cart.getId());
      Map<Integer, Product> idToProductMap = webSupport.getIdToProductMap(cartWithProducts.getProductIdSet());
      Map<Integer, Seller> idToSellerMap = webSupport.getIdToSellerMap(cartWithProducts.getSellerUserIdSet());

      request.setAttribute("CartWithProducts", cartWithProducts);
      request.setAttribute("IdToProductMap", idToProductMap);
      request.setAttribute("IdToSellerMap", idToSellerMap);
      
      return SUCCESS_FORWARD;
    }
    catch(Throwable throwable) {
      
      System.out.println("Error completing checkout.");
      throwable.printStackTrace();
      
      return FAILURE_FORWARD;
    }
  }
}
