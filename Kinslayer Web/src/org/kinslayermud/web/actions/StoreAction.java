package org.kinslayermud.web.actions;

import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts2.interceptor.ServletRequestAware;
import org.apache.struts2.interceptor.ServletResponseAware;

import org.kinslayermud.cart.Cart;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.util.WebSupportImp;
import com.opensymphony.xwork2.ActionSupport;
import org.kinslayermud.web.util.HttpUtil;

public abstract class StoreAction extends ActionSupport implements ServletRequestAware, ServletResponseAware {

  protected HttpServletRequest request;
  protected HttpServletResponse response;
  protected WebSupport webSupport;
  
  protected static final String CART_ID_COOKIE_NAME = "CartId";
  protected static final String CART_RANDOM_ID_COOKIE_NAME = "CartRandomId";
  
  public StoreAction() {

    webSupport = new WebSupportImp();
  }
  
  public abstract String execute(Cart cart) throws Exception;
  
  public String execute() throws Exception {
    
    Cart cart = null;
    
    Cookie cartIdCookie = HttpUtil.getCookieByName(request, CART_ID_COOKIE_NAME);
    Cookie cartRandomIdCookie = HttpUtil.getCookieByName(request, CART_RANDOM_ID_COOKIE_NAME);
    
    if(cartIdCookie != null && cartRandomIdCookie != null) {
      
      int cartId;
      String cartRandomId;
      
      if(MiscUtil.isValidIntString(cartIdCookie.getValue())) {
        
        cartId = Integer.valueOf(cartIdCookie.getValue());
        cartRandomId = cartRandomIdCookie.getValue();
        
        cart = webSupport.getCart(cartId);
        
        if(cart != null && !cart.getRandomId().equals(cartRandomId)) {
          
          cart = null;//Mismatch. Build a new cart.
        }
      }
    }
    
    if(cart == null) {
      
      cart = webSupport.createNewCart(request.getRemoteAddr());
      
      cartIdCookie = new Cookie(CART_ID_COOKIE_NAME, String.valueOf(cart.getId()));
      cartRandomIdCookie = new Cookie(CART_RANDOM_ID_COOKIE_NAME, cart.getRandomId());
      
      response.addCookie(cartIdCookie);
      response.addCookie(cartRandomIdCookie);
    }
    
    System.out.println("Store Action. Cart Found: " + cart.getId());
    
    return execute(cart);
  }
  
  public void setServletRequest(HttpServletRequest request) {

    this.request = request;
  }

  public void setServletResponse(HttpServletResponse response) {

    this.response = response;
  }
}
