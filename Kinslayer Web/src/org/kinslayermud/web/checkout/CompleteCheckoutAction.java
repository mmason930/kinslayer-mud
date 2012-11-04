package org.kinslayermud.web.checkout;

import javax.servlet.http.Cookie;

import org.kinslayermud.cart.Cart;
import org.kinslayermud.exception.DataInterfaceException;
import org.kinslayermud.web.actions.StoreAction;
import org.kinslayermud.web.util.HttpUtil;

public class CompleteCheckoutAction extends StoreAction {

  protected final String SUCCESS_FORWARD = "Success";
  protected final String FAILURE_FORWARD = "Failure";

  protected static final String CART_ID_COOKIE_NAME = "CartId";
  protected static final String CART_RANDOM_ID_COOKIE_NAME = "CartRandomId";
  
  public String execute(Cart cart) throws DataInterfaceException {
    
    webSupport.completeCheckout(cart.getId());
    
    System.out.println("Creating New Cart.");
    cart = webSupport.createNewCart(request.getRemoteAddr());
    
    Cookie cookie;
    
    cookie = new Cookie(CART_ID_COOKIE_NAME, String.valueOf(cart.getId()));
    response.addCookie(cookie);
    cookie = new Cookie(CART_RANDOM_ID_COOKIE_NAME, cart.getRandomId());
    response.addCookie(cookie);
    
    return SUCCESS_FORWARD;
  }
}
