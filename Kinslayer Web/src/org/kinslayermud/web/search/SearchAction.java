package org.kinslayermud.web.search;

import java.util.List;

import org.kinslayermud.cart.Cart;
import org.kinslayermud.product.ProductType;
import org.kinslayermud.search.ProductSearchResult;
import org.kinslayermud.web.actions.StoreAction;


public class SearchAction extends StoreAction {

  protected static final String SUCCESS_FORWARD = "Success";
  protected static final String FAILURE_FORWARD = "Failure";
  
  public String execute(Cart  cart) {

    try {
      System.out.println("SearchAction start!");
  
      ProductType productType = ProductType.grocery;
      int offset = 0;//TODO: Dynamic
      int fetchSize = 10;//TODO: Dynamic
      
      List<ProductSearchResult> productSearchResults = webSupport.performProductSearch(productType, offset, fetchSize);
  
      System.out.println("Total Products Found: " + productSearchResults.size());
      
      request.setAttribute("ProductSearchResults", productSearchResults);
      
      return SUCCESS_FORWARD;
    }
    catch(Throwable throwable) {
      
      throwable.printStackTrace();
    }
    
    return FAILURE_FORWARD;
  }
}
