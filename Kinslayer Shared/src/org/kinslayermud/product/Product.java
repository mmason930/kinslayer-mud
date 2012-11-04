package org.kinslayermud.product;

import java.util.Date;

import org.kinslayermud.misc.DataObjectWithIntId;


public abstract class Product extends DataObjectWithIntId {

  protected String productCode;
  protected String name;
  protected Integer listPrice;
  protected String asin;
  protected String manufacturer;
  protected Integer weight;
  protected String dimensions;
  protected Date createdDatetime;
  protected Date lastModifiedDatetime;
  
  public Product() {
    
    id = NEW;
  }
  
  public String getProductCode() {
    
    return productCode;
  }
  
  public void setProductCode(String productCode) {
    
    this.productCode = productCode;
  }
  
  public String getName() {
    
    return name;
  }
  
  public void setName(String name) {
    
    this.name = name;
  }
  
  public Integer getListPrice() {
    
    return listPrice;
  }
  
  public void setListPrice(Integer listPrice) {
    
    this.listPrice = listPrice;
  }
  
  public String getAsin() {
    
    return asin;
  }
  
  public void setAsin(String asin) {
    
    this.asin = asin;
  }
  
  public String getManufacturer() {
    
    return manufacturer;
  }
  
  public void setManufacturer(String manufacturer) {
    
    this.manufacturer = manufacturer;
  }
  
  public Integer getWeight() {
    
    return weight;
  }
  
  public void setWeight(Integer weight) {
    
    this.weight = weight;
  }

  public String getDimensions() {
    
    return dimensions;
  }
  
  public void setDimensions(String dimensions) {
    
    this.dimensions = dimensions;
  }
  
  public Date getCreatedDatetime() {
    
    return createdDatetime;
  }
  
  public void setCreatedDatetime(Date createdDatetime) {
    
    this.createdDatetime = createdDatetime;
  }
  
  public Date getLastModifiedDatetime() {
    
    return lastModifiedDatetime;
  }
  
  public void setLastModifiedDatetime(Date lastModifiedDatetime) {
    
    this.lastModifiedDatetime = lastModifiedDatetime;
  }
  
  public abstract ProductType getType();
}
