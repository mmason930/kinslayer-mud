package org.kinslayermud.search;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.kinslayermud.product.Product;
import org.kinslayermud.salelisting.SaleListing;
import org.kinslayermud.seller.Seller;

public class ProductSearchResult {

  protected List<SaleListing> saleListings = new LinkedList<SaleListing>();
  protected Map<Integer, Seller> idToSellerMap = new HashMap<Integer, Seller>();
  protected Product product;
  
  public List<SaleListing> getSaleListings() {
    
    return saleListings;
  }
  
  public void setSaleListings(List<SaleListing> saleListings) {
    
    this.saleListings = saleListings;
  }
  
  public Seller getSellerById(int sellerId) {
    
    return idToSellerMap.get(sellerId);
  }
  
  public void setIdToSellerMap(Map<Integer, Seller> idToSellerMap) {
    
    this.idToSellerMap = idToSellerMap;
  }
  
  public Product getProduct() {
    
    return product;
  }
  
  public void setProduct(Product product) {
    
    this.product = product;
  }
  
  public SaleListing getLowestPricedListing() {
    
    SaleListing lowestPricedListing = null;
    for(SaleListing saleListing : saleListings) {
      
      if(lowestPricedListing == null || saleListing.getPrice().compareTo(lowestPricedListing.getPrice()) < 0) {
        
        lowestPricedListing = saleListing;
      }
    }
    
    return lowestPricedListing;
  }
  
}
