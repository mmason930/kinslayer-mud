package org.kinslayermud.web.checkout;

import java.util.Map;

import org.kinslayermud.cart.Cart;
import org.kinslayermud.cart.CartWithProducts;
import org.kinslayermud.product.Product;
import org.kinslayermud.seller.Seller;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.web.actions.StoreAction;

public class AddCartSaleProductAction extends StoreAction {

  protected static final String SUCCESS_FORWARD = "Success";
  protected static final String FAILURE_FORWARD = "Failure";
  
  protected static final String SALE_LISTING_ID_PARAMETER = "SaleListingId";
  protected static final String QUANTITY_PARAMETER = "Quantity";
  
  public String execute(Cart cart) throws Exception {

    if(request.getParameter(SALE_LISTING_ID_PARAMETER) == null || request.getParameter(QUANTITY_PARAMETER) == null) {
      
      return FAILURE_FORWARD;
    }
    
    if(!MiscUtil.isValidIntString(request.getParameter(SALE_LISTING_ID_PARAMETER)) || !MiscUtil.isValidIntString(request.getParameter(QUANTITY_PARAMETER))) {
      
      return FAILURE_FORWARD;
    }
    
    int saleListingId = Integer.valueOf(request.getParameter(SALE_LISTING_ID_PARAMETER));
    short quantity = Short.valueOf(request.getParameter(QUANTITY_PARAMETER));
    
    webSupport.allocateSaleListingForCart(cart, saleListingId, quantity);
    
    return SUCCESS_FORWARD;
  }
}
