package org.kinslayermud.kit;

import org.kinslayermud.misc.DataObjectWithIntId;

public class KitItem extends DataObjectWithIntId {

  protected int kitId;
  protected int bodyPosition;
  protected int objectPrototypeId;
  protected int probability;
  protected int hierarchy;
  
  public int getKitId() {
    return kitId;
  }
  public void setKitId(int kitId) {
    this.kitId = kitId;
  }
  public int getBodyPosition() {
    return bodyPosition;
  }
  public void setBodyPosition(int bodyPosition) {
    this.bodyPosition = bodyPosition;
  }
  public int getObjectPrototypeId() {
    return objectPrototypeId;
  }
  public void setObjectPrototypeId(int objectPrototypeId) {
    this.objectPrototypeId = objectPrototypeId;
  }
  public int getProbability() {
    return probability;
  }
  public void setProbability(int probability) {
    this.probability = probability;
  }
  public int getHierarchy() {
    return hierarchy;
  }
  public void setHierarchy(int hierarchy) {
    this.hierarchy = hierarchy;
  }
  
  
}
