package org.kinslayermud.address;

public class NameAddress {

  protected String name;
  protected String addressLine1;
  protected String addressLine2;
  protected String city;
  protected String region;
  protected String postalCode;
  protected String country;
  
  public static NameAddress clone(NameAddress sourceNameAddress) {
    
    NameAddress newNameAddress = new NameAddress();
    
    newNameAddress.setName(sourceNameAddress.getName());
    newNameAddress.setAddressLine1(sourceNameAddress.getAddressLine1());
    newNameAddress.setAddressLine2(sourceNameAddress.getAddressLine2());
    newNameAddress.setCity(sourceNameAddress.getCity());
    newNameAddress.setRegion(sourceNameAddress.getRegion());
    newNameAddress.setPostalCode(sourceNameAddress.getPostalCode());
    newNameAddress.setCountry(sourceNameAddress.getCountry());
    
    return newNameAddress;
  }
  
  public String getName() {
    
    return name;
  }
  
  public void setName(String name) {
    
    this.name = name;
  }
  
  public String getAddressLine1() {
    
    return addressLine1;
  }
  
  public void setAddressLine1(String addressLine1) {
    
    this.addressLine1 = addressLine1;
  }
  
  public String getAddressLine2() {
    
    return addressLine2;
  }
  
  public void setAddressLine2(String addressLine2) {
    
    this.addressLine2 = addressLine2;
  }
  
  public String getCity() {
    
    return city;
  }
  
  public void setCity(String city) {
    
    this.city = city;
  }
  
  public String getRegion() {
    
    return region;
  }
  
  public void setRegion(String region) {
    
    this.region = region;
  }
  
  public String getPostalCode() {
    
    return postalCode;
  }
  
  public void setPostalCode(String postalCode) {
    
    this.postalCode = postalCode;
  }
  
  public String getCountry() {
    
    return country;
  }
  
  public void setCountry(String country) {
    
    this.country = country;
  }
}
