package org.kinslayermud.clan;

import org.kinslayermud.misc.DataObjectWithIntId;

public class Clan extends DataObjectWithIntId {

  protected String name;
  protected boolean isSecret;
  protected int warrantId;
  protected int hiddenLevel;
  protected int bonusSkillId;
  protected int bonusSkillModifier;
  protected int bonusSkillRank;
  
  public Clan() {
    
    id = NEW;
  }
  
  public String getName() {
    return name;
  }
  public void setName(String name) {
    this.name = name;
  }
  public boolean isSecret() {
    return isSecret;
  }
  public void setSecret(boolean isSecret) {
    this.isSecret = isSecret;
  }
  public int getWarrantId() {
    return warrantId;
  }
  public void setWarrantId(int warrantId) {
    this.warrantId = warrantId;
  }
  public int getHiddenLevel() {
    return hiddenLevel;
  }
  public void setHiddenLevel(int hiddenLevel) {
    this.hiddenLevel = hiddenLevel;
  }
  public int getBonusSkillId() {
    return bonusSkillId;
  }
  public void setBonusSkillId(int bonusSkillId) {
    this.bonusSkillId = bonusSkillId;
  }
  public int getBonusSkillModifier() {
    return bonusSkillModifier;
  }
  public void setBonusSkillModifier(int bonusSkillModifier) {
    this.bonusSkillModifier = bonusSkillModifier;
  }
  public int getBonusSkillRank() {
    return bonusSkillRank;
  }
  public void setBonusSkillRank(int bonusSkillRank) {
    this.bonusSkillRank = bonusSkillRank;
  }
}
