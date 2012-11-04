package org.kinslayermud.web.checkout;

import org.kinslayermud.cart.Cart;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.web.actions.StoreAction;

public class RemoveFromCartAction extends StoreAction {

  protected static final String SUCCESS_FORWARD = "Success";
  protected static final String FAILURE_FORWARD = "Failure";
  
  protected static final String CART_SALE_PRODUCT_ID_PARAMETER = "CartSaleProductId";
  
  public String execute(Cart cart) throws Exception {

    if(request.getParameter(CART_SALE_PRODUCT_ID_PARAMETER) == null) {
      
      return FAILURE_FORWARD;
    }
    
    if(!MiscUtil.isValidIntString(request.getParameter(CART_SALE_PRODUCT_ID_PARAMETER))) {
      
      return FAILURE_FORWARD;
    }
    
    int cartSaleProductId = Integer.valueOf(request.getParameter(CART_SALE_PRODUCT_ID_PARAMETER));
    
    webSupport.removeCartItem(cart.getId(), cartSaleProductId);
    
    return SUCCESS_FORWARD;
  }
}
