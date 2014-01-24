package org.kinslayermud.object;

import java.math.BigDecimal;
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.Set;

import org.kinslayermud.misc.DataObjectWithIntId;

public class ObjectPrototype extends DataObjectWithIntId {

  protected String shortDescription;
  protected int offensiveBonus;
  protected int dodgeBonus;
  protected int parryBonus;
  protected int absorb;
  protected BigDecimal weight;
  protected int wear;
  protected int[] values = new int[12];
  
  public String getShortDescription() {
    return shortDescription;
  }
  public void setShortDescription(String shortDescription) {
    this.shortDescription = shortDescription;
  }
  public int getOffensiveBonus() {
    return offensiveBonus;
  }
  public void setOffensiveBonus(int offensiveBonus) {
    this.offensiveBonus = offensiveBonus;
  }
  public int getDodgeBonus() {
    return dodgeBonus;
  }
  public void setDodgeBonus(int dodgeBonus) {
    this.dodgeBonus = dodgeBonus;
  }
  public int getParryBonus() {
    return parryBonus;
  }
  public void setParryBonus(int parryBonus) {
    this.parryBonus = parryBonus;
  }
  public int getAbsorb() {
    return absorb;
  }
  public void setAbsorb(int absorb) {
    this.absorb = absorb;
  }
  public BigDecimal getWeight() {
    return weight;
  }
  public void setWeight(BigDecimal weight) {
    this.weight = weight;
  }
  
  public int getWear() {
    
    return wear;
  }
  
  public void setWear(int wear) {
    
    this.wear = wear;
  }
  
  public Set<ObjectWearType> getObjectWearTypes() {
    
    Set<ObjectWearType> objectWearTypes = new LinkedHashSet<ObjectWearType>();
    
    Iterator<ObjectWearType> iter = ObjectWearType.getSetIterator();
    while(iter.hasNext()) {
      
      ObjectWearType objectWearType = iter.next();
      
      if((wear & (1 << objectWearType.value())) != 0) {
        
        objectWearTypes.add(objectWearType);
      }
    }
    
    return objectWearTypes;
  }
  
  public int getValue(int number) {
    
    return values[ number ];
  }
  
  public void setValue(int number, int value) {
    
    this.values[ number ] = value;
  }
}
