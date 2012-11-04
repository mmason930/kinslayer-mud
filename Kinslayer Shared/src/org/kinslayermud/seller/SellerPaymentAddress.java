package org.kinslayermud.seller;

import org.kinslayermud.address.NameAddress;
import org.kinslayermud.misc.DataObjectWithIntId;

public class SellerPaymentAddress extends DataObjectWithIntId {

  protected int sellerId;
  protected NameAddress address;
  
  public SellerPaymentAddress() {
    
    id = NEW;
  }
  
  public int getSellerId() {
    
    return sellerId;
  }
  
  public void setSellerId(int sellerId) {
    
    this.sellerId = sellerId;
  }
  
  public NameAddress getAddress() {
    
    return address;
  }
  
  public void setAddress(NameAddress address) {
    
    this.address = address;
  }
}
