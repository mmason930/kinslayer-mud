package org.kinslayermud.web.checkout;

import java.math.BigDecimal;
import java.util.LinkedList;
import java.util.List;

import org.kinslayermud.cart.Cart;
import org.kinslayermud.shipping.ShippingType;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.web.actions.StoreAction;

public class SetItemShippingAction extends StoreAction {

  protected static final String SUCCESS_FORWARD = "Success";
  protected static final String FAILURE_FORWARD = "Failure";
  
  protected static final String SHIPPING_TYPE_PARAMETER = "ShippingType";
  
  public class JsonResponse {
    
    public String message;
    public BigDecimal cartTotal;
    
    public List<JsonResponseCartSaleProducts> jsonResponseCartSaleProducts = new LinkedList<JsonResponseCartSaleProducts>();
  }
  
  public class JsonResponseCartSaleProducts {
    
    int cartSaleProductId;
    short quantity;
    BigDecimal itemAmount;
    BigDecimal shippingAmount;
  }
  
  public String execute(Cart cart) throws Exception {

    System.out.println("SetItemShippingAction Start!");
    System.out.println("SHIPPING TYPE PARAMETER: " + request.getParameter(SHIPPING_TYPE_PARAMETER));
    
    JsonResponse jsonResponse = new JsonResponse();
    jsonResponse.message = "Failure";
    request.setAttribute("JSONObject", jsonResponse);
    
    if(request.getParameter(SHIPPING_TYPE_PARAMETER) == null) {
      
      return FAILURE_FORWARD;
    }
    
    if(!MiscUtil.isValidIntString(request.getParameter(SHIPPING_TYPE_PARAMETER))) {
      
      return FAILURE_FORWARD;
    }
    
    int shippingTypeValue = Integer.valueOf(request.getParameter(SHIPPING_TYPE_PARAMETER));
    ShippingType shippingType = ShippingType.getEnum(shippingTypeValue);

    
    System.out.println("Shipping Type Value: " + shippingTypeValue);
    System.out.println("Shipping Type Name : " + shippingType.getStandardName());
    
    jsonResponse.message = "Success";
    return SUCCESS_FORWARD;
  }
}
