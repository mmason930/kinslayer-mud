package org.kinslayermud.misc;

import java.io.Serializable;

public class DataObjectWithIntId implements Serializable {
  
  protected int id;
  protected static int NEW = -1;
  
  public DataObjectWithIntId() {
    
    id = NEW;
  }
  
  public boolean isNew() {
    
    return id == NEW;
  }
  
  public int getId() {
    
    return id;
  }
  
  public void setId(int id) {
    
    this.id = id;
  }
}
