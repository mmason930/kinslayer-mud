package org.kinslayermud.object;

import java.math.BigDecimal;

import org.kinslayermud.misc.DataObjectWithIntId;

public class ObjectPrototype extends DataObjectWithIntId {

  protected String shortDescription;
  protected int offensiveBonus;
  protected int dodgeBonus;
  protected int parryBonus;
  protected int absorb;
  protected BigDecimal weight;
  
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
}
