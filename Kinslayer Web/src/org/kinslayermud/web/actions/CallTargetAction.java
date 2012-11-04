package org.kinslayermud.web.actions;

import org.kinslayermud.cart.Cart;


public class CallTargetAction extends StoreAction {

  public static final String FORARD_NAME_PARAMETER = "forward_name";
  
  protected String forward;
  
  public String getForward() {
    
    return forward;
  }
  
  public String execute(Cart  cart) {
    
    forward = request.getParameter(FORARD_NAME_PARAMETER);
    System.out.println("Forwarding to `" + forward + "`");
    return "Redirect";
  }
}