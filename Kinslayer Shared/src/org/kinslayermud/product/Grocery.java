package org.kinslayermud.product;

public class Grocery extends Product {

  protected String brand;
  protected Integer numberOfItems;
  
  public String getBrand() {
    
    return brand;
  }
  
  public void setBrand(String brand) {
    
    this.brand = brand;
  }
  
  public Integer getNumberOfItems() {
    
    return numberOfItems;
  }
  
  public void setNumberOfItems(Integer numberOfItems) {
    
    this.numberOfItems = numberOfItems;
  }
  
  public ProductType getType() {
    
    return ProductType.grocery;
  }
}
