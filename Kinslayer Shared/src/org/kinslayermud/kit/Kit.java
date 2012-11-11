package org.kinslayermud.kit;

import org.kinslayermud.misc.DataObjectWithIntId;

public class Kit extends DataObjectWithIntId {

  protected String name;
  
  public String getName() {
    
    return name;
  }
  
  public void setName(String name) {
    
    this.name = name;
  }
}
