package org.kinslayermud.web.actions;

import org.kinslayermud.cart.Cart;

public class DefaultAction extends StoreAction {

  private static String REDIRECT_FORWARD = "Redirect";
  private static String FAILURE_FORWARD = "Failure";
  
  public String execute(Cart  cart) {
    
    if(request.getRequestURI().equals("/")) {
      
      return REDIRECT_FORWARD;
    }
    
    return FAILURE_FORWARD;
  }
}
