package org.kinslayermud.seller;

import java.math.BigDecimal;
import java.util.Date;

import org.kinslayermud.misc.DataObjectWithIntId;
import org.kinslayermud.user.User;

public class Seller extends User {

  protected String username;
  protected SellerStatus status;
  protected Date statusLastModifiedDatetime;
  protected BigDecimal commissionRate;
  protected Date lastLoginDatetime;
  protected int totalOrders;
  protected int totalOrderItems;
  protected int totalShippedItems;
  protected int totalCancelledItems;
  
  public Seller() {
    
    super();
  }
  
  public String getUsername() {
    
    return username;
  }
  
  public void setUsername(String username) {
    
    this.username = username;
  }
  
  public SellerStatus getStatus() {
    
    return status;
  }
  
  public void setStatus(SellerStatus status) {
    
    this.status = status;
  }
  
  public Date getStatusLastModifiedDatetime() {
    
    return statusLastModifiedDatetime;
  }
  
  public void setStatusLastModifiedDatetime(Date statusLastModifiedDatetime) {
    
    this.statusLastModifiedDatetime = statusLastModifiedDatetime;
  }
  
  public BigDecimal getCommissionRate() {
    
    return commissionRate;
  }
  
  public void setCommissionRate(BigDecimal commissionRate) {
    
    this.commissionRate = commissionRate;
  }
  
  public Date getLastLoginDatetime() {
    
    return lastLoginDatetime;
  }
  
  public void setLastLoginDatetime(Date lastLoginDatetime) {
    
    this.lastLoginDatetime = lastLoginDatetime;
  }
  
  public int getTotalOrders() {
    
    return totalOrders;
  }
  
  public void setTotalOrders(int totalOrders) {
    
    this.totalOrders = totalOrders;
  }
  
  public int getTotalOrderItems() {
    
    return totalOrderItems;
  }
  
  public void setTotalOrderItems(int totalOrderItems) {
    
    this.totalOrderItems = totalOrderItems;
  }
  
  public int getTotalShippedItems() {
    
    return totalShippedItems;
  }
  
  public void setTotalShippedItems(int totalShippedItems) {
    
    this.totalShippedItems = totalShippedItems;
  }
  
  public int getTotalCancelledItems() {
    
    return totalCancelledItems;
  }
  
  public void setTotalCancelledItems(int totalCancelledItems) {
    
    this.totalCancelledItems = totalCancelledItems;
  }
}
