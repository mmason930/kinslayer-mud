package org.kinslayermud.cart;

import java.util.Date;

import org.kinslayermud.address.NameAddress;
import org.kinslayermud.misc.DataObjectWithIntId;
import org.kinslayermud.payment.PaymentMethodType;

public class Cart extends DataObjectWithIntId {

  protected String ipAddress;
  protected Date createdDatetime;
  protected String emailAddress;
  protected Date lastModifiedDatetime;
  protected PaymentMethodType paymentMethodType;
  protected String paymentToken;
  protected String payerId;
  protected String randomId;
  protected String phoneNumber;
  protected NameAddress billingAddress;
  protected NameAddress shippingAddress;
  
  public boolean isShippingAndPaymentComplete() {
    
    if(shippingAddress == null) {
      
      return false;
    }
    
    if(emailAddress == null) {
      
      return false;
    }
    
    if(getPaymentMethodType().equals(PaymentMethodType.paypalExpress) && (paymentToken == null || payerId == null)) {
      
      return false;
    }
    
    return true;
  }
  
  public String getIpAddress() {
    
    return ipAddress;
  }
  
  public void setIpAddress(String ipAddress) {
    
    this.ipAddress = ipAddress;
  }
  
  public Date getCreatedDatetime() {
    
    return createdDatetime;
  }
  
  public void setCreatedDatetime(Date createdDatetime) {
    
    this.createdDatetime = createdDatetime;
  }
  
  public String getEmailAddress() {
    
    return emailAddress;
  }
  
  public void setEmailAddress(String emailAddress) {
    
    this.emailAddress = emailAddress;
  }
  
  public String getRandomId() {
    
    return randomId;
  }
  
  public void setRandomId(String randomId) {
    
    this.randomId = randomId;
  }
  
  public String getPhoneNumber() {
    
    return phoneNumber;
  }
  
  public void setPhoneNumber(String phoneNumber) {
    
    this.phoneNumber = phoneNumber;
  }
  
  public NameAddress getBillingAddress() {
    
    return billingAddress;
  }
  
  public void setBillingAddress(NameAddress billingAddress) {
    
    this.billingAddress = billingAddress;
  }
  
  public NameAddress getShippingAddress() {
    
    return shippingAddress;
  }
  
  public void setShippingAddress(NameAddress shippingAddress) {
    
    this.shippingAddress = shippingAddress;
  }

  public PaymentMethodType getPaymentMethodType() {
    return paymentMethodType;
  }

  public void setPaymentMethodType(PaymentMethodType paymentMethodType) {
    this.paymentMethodType = paymentMethodType;
  }

  public String getPaymentToken() {
    return paymentToken;
  }

  public void setPaymentToken(String paymentToken) {
    this.paymentToken = paymentToken;
  }

  public String getPayerId() {
    return payerId;
  }

  public void setPayerId(String payerId) {
    this.payerId = payerId;
  }

  public Date getLastModifiedDatetime() {
    return lastModifiedDatetime;
  }

  public void setLastModifiedDatetime(Date lastModifiedDatetime) {
    this.lastModifiedDatetime = lastModifiedDatetime;
  }
}
