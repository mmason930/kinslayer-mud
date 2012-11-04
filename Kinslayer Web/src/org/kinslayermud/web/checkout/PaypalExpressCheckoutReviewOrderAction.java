package org.kinslayermud.web.checkout;

import org.kinslayermud.cart.Cart;
import org.kinslayermud.exception.DataInterfaceException;
import org.kinslayermud.web.actions.StoreAction;

public class PaypalExpressCheckoutReviewOrderAction extends StoreAction {

  protected static final String SUCCESS_FORWARD = "Success";
  protected static final String FAILURE_FORWARD = "Failure";
  
  protected static final String PAYPAL_TOKEN_PARAMETER = "token";
  
  public String execute(Cart cart) throws DataInterfaceException {
   
    System.out.println("BEGIN PaypalExpressCheckoutReviewOrderAction!");
    String token = request.getParameter(PAYPAL_TOKEN_PARAMETER);
    
    if(token == null) {
      
      return FAILURE_FORWARD;
    }
    
    System.out.println("Token: " + token);
    
    webSupport.getExpressCheckoutDetails(cart.getId(), token);
    
    return SUCCESS_FORWARD;
  }
}
