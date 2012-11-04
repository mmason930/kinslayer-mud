package org.kinslayermud.saletransaction;

import java.util.Date;

import org.kinslayermud.address.NameAddress;
import org.kinslayermud.misc.DataObjectWithIntId;
import org.kinslayermud.payment.PaymentMethodType;

public class SaleTransaction extends DataObjectWithIntId {

  protected String emailAddress;
  protected String ipAddress;
  protected Date createdDatetime;
  protected String name;
  protected SaleTransactionStatus status;
  protected Date statusLastModifiedDatetime;
  protected Date autoCancelDatetime;
  protected String phoneNumber;
  protected String randomId;
  protected PaymentMethodType paymentMethodType;
  protected int paymentId;
  protected NameAddress shippingAddress;
  
  public SaleTransaction() {
    
    id = NEW;
  }
  
  public String getName() {
    
    return name;
  }
  
  public void setName(String name) {
    
    this.name = name;
  }
  
  public String getEmailAddress() {
    return emailAddress;
  }
  public void setEmailAddress(String emailAddress) {
    this.emailAddress = emailAddress;
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
  public SaleTransactionStatus getStatus() {
    return status;
  }
  public void setStatus(SaleTransactionStatus status) {
    this.status = status;
  }
  public Date getStatusLastModifiedDatetime() {
    return statusLastModifiedDatetime;
  }
  public void setStatusLastModifiedDatetime(Date statusLastModifiedDatetime) {
    this.statusLastModifiedDatetime = statusLastModifiedDatetime;
  }
  public Date getAutoCancelDatetime() {
    return autoCancelDatetime;
  }
  public void setAutoCancelDatetime(Date autoCancelDatetime) {
    this.autoCancelDatetime = autoCancelDatetime;
  }
  public String getPhoneNumber() {
    return phoneNumber;
  }
  public void setPhoneNumber(String phoneNumber) {
    this.phoneNumber = phoneNumber;
  }
  public String getRandomId() {
    return randomId;
  }
  public void setRandomId(String randomId) {
    this.randomId = randomId;
  }
  public PaymentMethodType getPaymentMethodType() {
    return paymentMethodType;
  }
  public void setPaymentMethodType(PaymentMethodType paymentMethodType) {
    this.paymentMethodType = paymentMethodType;
  }
  public int getPaymentId() {
    return paymentId;
  }
  public void setPaymentId(int paymentId) {
    this.paymentId = paymentId;
  }
  public NameAddress getShippingAddress() {
    return shippingAddress;
  }
  public void setShippingAddress(NameAddress shippingAddress) {
    this.shippingAddress = shippingAddress;
  }
  
}
