package org.kinslayermud.web.checkout;

import java.io.IOException;
import java.math.BigDecimal;

import org.kinslayermud.cart.Cart;
import org.kinslayermud.cart.CartWithProducts;
import org.kinslayermud.exception.DataInterfaceException;
import org.kinslayermud.web.actions.StoreAction;

public class PaypalExpressCheckoutAction extends StoreAction {

  protected static final String SUCCESS_FORWARD = "Success";
  protected static final String FAILURE_FORWARD = "Failure";
  
  public String execute(Cart cart) throws DataInterfaceException, IOException {
    
    CartWithProducts cartWithProducts = webSupport.getCartWithProducts(cart.getId());

    BigDecimal totalCheckoutAmount = cartWithProducts.getTotalAmount();
    
    System.out.println("Total Checkout Amount: " + totalCheckoutAmount);
    
    String token = webSupport.performSetExpressCheckout(cart.getId(), totalCheckoutAmount);
    
    String destinationUrl = "https://www.sandbox.paypal.com/cgi-bin/webscr?cmd=_express-checkout&token=" + token;
    
    System.out.println("Paypal Express Checkout URL: " + destinationUrl);
    
    response.sendRedirect(destinationUrl);
    
    return SUCCESS_FORWARD;
  }
}
