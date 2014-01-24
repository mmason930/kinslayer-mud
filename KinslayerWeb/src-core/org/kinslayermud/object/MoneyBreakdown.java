package org.kinslayermud.object;

public class MoneyBreakdown {

  protected int gold;
  protected int silver;
  protected int copper;
  
  public MoneyBreakdown(long total) {
    
    copper = (int) (total % 100);
    silver = (int) (total / 100) % 100;
    gold   = (int) (total / 10000);
  }
  
  public int getGold() {
    
    return gold;
  }
  
  public void setGold(int gold) {
    
    this.gold = gold;
  }
  
  public int getSilver() {
    
    return silver;
  }
  
  public void setSilver(int silver) {
    
    this.silver = silver;
  }
  
  public int getCopper() {
    
    return copper;
  }
  
  public void setCopper(int copper) {
    
    this.copper = copper;
  }
}
