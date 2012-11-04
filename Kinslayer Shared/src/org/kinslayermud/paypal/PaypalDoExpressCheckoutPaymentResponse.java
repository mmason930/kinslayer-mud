package org.kinslayermud.paypal;

import java.math.BigDecimal;

public class PaypalDoExpressCheckoutPaymentResponse {

  protected String acknowledgement;
  protected String version;
  protected String build;
  protected String token;
  protected String transactionId;
  protected String transactionType;
  protected String paymentType;
  protected BigDecimal amount;
  protected BigDecimal taxAmount;
  protected String pendingReason;
  protected String reasonCode;
  
  public String getAcknowledgement() {
    return acknowledgement;
  }
  public void setAcknowledgement(String acknowledgement) {
    this.acknowledgement = acknowledgement;
  }
  public String getVersion() {
    return version;
  }
  public void setVersion(String version) {
    this.version = version;
  }
  public String getBuild() {
    return build;
  }
  public void setBuild(String build) {
    this.build = build;
  }
  public String getToken() {
    return token;
  }
  public void setToken(String token) {
    this.token = token;
  }
  public String getTransactionId() {
    return transactionId;
  }
  public void setTransactionId(String transactionId) {
    this.transactionId = transactionId;
  }
  public String getTransactionType() {
    return transactionType;
  }
  public void setTransactionType(String transactionType) {
    this.transactionType = transactionType;
  }
  public String getPaymentType() {
    return paymentType;
  }
  public void setPaymentType(String paymentType) {
    this.paymentType = paymentType;
  }
  public BigDecimal getAmount() {
    return amount;
  }
  public void setAmount(BigDecimal amount) {
    this.amount = amount;
  }
  public BigDecimal getTaxAmount() {
    return taxAmount;
  }
  public void setTaxAmount(BigDecimal taxAmount) {
    this.taxAmount = taxAmount;
  }
  public String getPendingReason() {
    return pendingReason;
  }
  public void setPendingReason(String pendingReason) {
    this.pendingReason = pendingReason;
  }
  public String getReasonCode() {
    return reasonCode;
  }
  public void setReasonCode(String reasonCode) {
    this.reasonCode = reasonCode;
  }
  
}
