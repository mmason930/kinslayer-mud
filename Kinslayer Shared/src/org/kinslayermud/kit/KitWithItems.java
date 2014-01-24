package org.kinslayermud.kit;

import java.util.ArrayList;
import java.util.List;

public class KitWithItems {

  protected Kit kit;
  protected List< List<KitItem> > kitItemLists = new ArrayList< List<KitItem> >();
  
  public Kit getKit() {
    
    return kit;
  }
  
  public void setKit(Kit kit) {
    
    this.kit = kit;
  }
  
  public List< List<KitItem> > getKitItemLists() {
    
    return kitItemLists;
  }
  
  public void setKitItemLists(List< List<KitItem> > kitItemLists) {
    
    this.kitItemLists = kitItemLists;
  }
}
