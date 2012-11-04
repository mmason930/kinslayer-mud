package org.kinslayermud.web.search;

import org.kinslayermud.cart.Cart;
import org.kinslayermud.product.Product;
import org.kinslayermud.salelisting.SaleListing;
import org.kinslayermud.seller.Seller;
import org.kinslayermud.web.actions.StoreAction;


public class ViewListingAction extends StoreAction {

  protected static final String SUCCESS_FORWARD = "Success";
  protected static final String FAILURE_FORWARD = "Failure";
  protected static final String LISTING_NOT_FOUND_FORWARD = "ListingNotFound";
  
  protected static final String SALE_LISTING_ID_PARAMETER = "SaleListingId";
  
  public String execute(Cart  cart) {

    try {
      System.out.println("ViewListingAction start!");
  
      if(request.getParameter(SALE_LISTING_ID_PARAMETER) == null) {
        
        return FAILURE_FORWARD;
      }
      
      int saleListingId = Integer.parseInt(request.getParameter(SALE_LISTING_ID_PARAMETER));
      SaleListing saleListing = webSupport.getSaleListing(saleListingId, false);

      if(saleListing == null) {
        
        return LISTING_NOT_FOUND_FORWARD;
      }
      
      Product product = webSupport.getProduct(saleListing.getProductId());
      Seller seller = webSupport.getSeller(saleListing.getSellerUserId());
      
      request.setAttribute("SaleListing", saleListing);
      request.setAttribute("Product", product);
      request.setAttribute("Seller", seller);
      
      return SUCCESS_FORWARD;
    }
    catch(Throwable throwable) {
      
      throwable.printStackTrace();
    }
    
    return FAILURE_FORWARD;
  }
}
