package org.kinslayermud.object;

public class Obj {

  protected String id;
  protected String shortDescription;
  protected int vnum;
  
  public String getId() {
    
    return id;
  }
  
  public void setId(String id) {
    
    this.id = id;
  }
  
  public String getShortDescription() {
    
    return shortDescription;
  }
  
  public void setShortDescription(String shortDescription) {
    
    this.shortDescription = shortDescription;
  }
  
  public int getVnum() {
    
    return vnum;
  }
  
  public void setVnum(int vnum) {
    
    this.vnum = vnum;
  }
}
