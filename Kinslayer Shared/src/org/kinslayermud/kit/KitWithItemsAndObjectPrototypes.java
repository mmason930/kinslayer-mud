package org.kinslayermud.kit;

import java.util.Map;

import org.kinslayermud.object.ObjectPrototype;

public class KitWithItemsAndObjectPrototypes {

  protected KitWithItems kitWithItems;
  protected Map<Integer, ObjectPrototype> idToObjectPrototypeMap;
  
  public KitWithItems getKitWithItems() {
    
    return kitWithItems;
  }
  
  public void setKitWithItems(KitWithItems kitWithItems) {
    
    this.kitWithItems = kitWithItems;
  }
  
  public Map<Integer, ObjectPrototype> getIdToObjectPrototypeMap() {
    
    return idToObjectPrototypeMap;
  }
  
  public void setIdToObjectPrototypeMap(Map<Integer, ObjectPrototype> idToObjectPrototypeMap) {
    
    this.idToObjectPrototypeMap = idToObjectPrototypeMap;
  }
}
