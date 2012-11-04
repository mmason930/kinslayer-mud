package org.kinslayermud.paypal;

import java.math.BigDecimal;

import org.kinslayermud.address.NameAddress;

public class PaypalGetExpressCheckoutDetailsResponse {

  protected String correlationId;
  protected String acknowledgement;
  protected String version;
  protected String build;
  protected String token;
  protected String emailAddress;
  protected String payerId;
  protected String payerStatus;
  protected String firstName;
  protected String lastName;
  protected String countryCode;
  protected String businessName;
  protected String shippingName;
  protected String shippingStreetAddressLine1;
  protected String shippingStreetAddressLine2;
  protected String shippingCity;
  protected String shippingState;
  protected String shippingCountryCode;
  protected String shippingCountryName;
  protected String shippingPostalCode;
  protected BigDecimal amount;
  
  public NameAddress getShippingAddress() {
    
    NameAddress shippingAddress = new NameAddress();
    
    shippingAddress.setName(shippingName);
    shippingAddress.setAddressLine1(shippingStreetAddressLine1);
    shippingAddress.setAddressLine2(shippingStreetAddressLine2);
    shippingAddress.setCity(shippingCity);
    shippingAddress.setRegion(shippingState);
    shippingAddress.setCountry(shippingCountryCode);
    shippingAddress.setPostalCode(shippingPostalCode);
    
    return shippingAddress;
  }
  
  public BigDecimal getAmount() {
    return amount;
  }
  public void setAmount(BigDecimal amount) {
    this.amount = amount;
  }
  public String getCorrelationId() {
    return correlationId;
  }
  public void setCorrelationId(String correlationId) {
    this.correlationId = correlationId;
  }
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
  public String getEmailAddress() {
    return emailAddress;
  }
  public void setEmailAddress(String emailAddress) {
    this.emailAddress = emailAddress;
  }
  public String getPayerId() {
    return payerId;
  }
  public void setPayerId(String payerId) {
    this.payerId = payerId;
  }
  public String getPayerStatus() {
    return payerStatus;
  }
  public void setPayerStatus(String payerStatus) {
    this.payerStatus = payerStatus;
  }
  public String getFirstName() {
    return firstName;
  }
  public void setFirstName(String firstName) {
    this.firstName = firstName;
  }
  public String getLastName() {
    return lastName;
  }
  public void setLastName(String lastName) {
    this.lastName = lastName;
  }
  public String getCountryCode() {
    return countryCode;
  }
  public void setCountryCode(String countryCode) {
    this.countryCode = countryCode;
  }
  public String getBusinessName() {
    return businessName;
  }
  public void setBusinessName(String businessName) {
    this.businessName = businessName;
  }
  public String getShippingName() {
    return shippingName;
  }
  public void setShippingName(String shippingName) {
    this.shippingName = shippingName;
  }
  public String getShippingStreetAddressLine1() {
    return shippingStreetAddressLine1;
  }
  public void setShippingStreetAddressLine1(String shippingStreetAddressLine1) {
    this.shippingStreetAddressLine1 = shippingStreetAddressLine1;
  }
  public String getShippingCity() {
    return shippingCity;
  }
  public void setShippingCity(String shippingCity) {
    this.shippingCity = shippingCity;
  }
  public String getShippingCountryCode() {
    return shippingCountryCode;
  }
  public void setShippingCountryCode(String shippingCountryCode) {
    this.shippingCountryCode = shippingCountryCode;
  }
  public String getShippingCountryName() {
    return shippingCountryName;
  }
  public void setShippingCountryName(String shippingCountryName) {
    this.shippingCountryName = shippingCountryName;
  }
  public String getShippingPostalCode() {
    return shippingPostalCode;
  }
  public void setShippingPostalCode(String shippingPostalCode) {
    this.shippingPostalCode = shippingPostalCode;
  }
  public String getShippingState() {
    return shippingState;
  }

  public void setShippingState(String shippingState) {
    this.shippingState = shippingState;
  }
  public String getShippingStreetAddressLine2() {
    return shippingStreetAddressLine2;
  }
  public void setShippingStreetAddressLine2(String shippingStreetAddressLine2) {
    this.shippingStreetAddressLine2 = shippingStreetAddressLine2;
  }
}
