package org.kinslayermud.salelisting;

import java.math.BigDecimal;

import org.kinslayermud.misc.DataObjectWithIntId;

public class SaleListing extends DataObjectWithIntId {

  protected int productId;
  protected int sellerUserId;
  protected BigDecimal price;
  protected String sku;
  protected short totalCount;
  protected short availableCount;

  public int getProductId() {
    
    return productId;
  }
  
  public void setProductId(int productId) {
    
    this.productId = productId;
  }
  
  public int getSellerUserId() {
    
    return sellerUserId;
  }
  
  public void setSellerUserId(int sellerUserId) {
    
    this.sellerUserId = sellerUserId;
  }
  
  public BigDecimal getPrice() {
    
    return price;
  }
  
  public void setPrice(BigDecimal price) {
    
    this.price = price;
  }
  
  public String getSku() {
    
    return sku;
  }
  
  public void setSku(String sku) {
    
    this.sku = sku;
  }
  
  public short getTotalCount() {
    
    return totalCount;
  }
  
  public void setTotalCount(short totalCount) {
    
    this.totalCount = totalCount;
  }
  
  public short getAvailableCount() {
    
    return availableCount;
  }
  
  public void setAvailableCount(short availableCount) {
    
    this.availableCount = availableCount;
  }
}
