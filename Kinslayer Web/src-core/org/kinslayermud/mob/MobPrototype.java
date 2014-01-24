package org.kinslayermud.mob;

import org.kinslayermud.misc.DataObjectWithIntId;

public class MobPrototype extends DataObjectWithIntId {

  protected String alias;
  protected String shortDescription;
  protected String longDescription;
  protected String description;
  protected String arriveMessage;
  protected String leaveMessage;
  protected int mobFlags;
  protected int affectedFlags0;
  protected int affectedFlags1;
  protected int affectedFlags2;
  protected int affectedFlags3;
  protected int level;
  protected int maxHit;
  protected int maxMana;
  protected int numberOfDamageDice;
  protected int damageDiceSize;
  protected int numerOfHitPointDice;
  protected int hitPointDiceSize;
  protected int damageRoll;
  protected int experience;
  protected int position;
  protected int defaultPosition;
  protected int sex;
  protected int race;
  protected int offensiveBonus;
  protected int parryBonus;
  protected int dodgeBonus;
  protected int clanId;
  protected int rank;
  protected int mobClass;
  protected int primaryKit;
  protected int foodObjectId;
  protected int foodQuantity;
  protected int skinObjectId;
  protected int skinRequired;
  protected int skinSkinned;
  protected int warrants0;
  protected int warrants1;
  protected int warrants2;
  protected int warrants3;
  protected int gold;
  protected int aggro;
  protected int nsects;
  protected int attack;
  protected int strength;
  protected int dexterity;
  protected int intelligence;
  protected int wisdom;
  protected int constitution;
  protected String assists;
  
  public String getAlias() {
    return alias;
  }
  public void setAlias(String alias) {
    this.alias = alias;
  }
  public String getShortDescription() {
    return shortDescription;
  }
  public void setShortDescription(String shortDescription) {
    this.shortDescription = shortDescription;
  }
  public String getLongDescription() {
    return longDescription;
  }
  public void setLongDescription(String longDescription) {
    this.longDescription = longDescription;
  }
  public String getDescription() {
    return description;
  }
  public void setDescription(String description) {
    this.description = description;
  }
  public String getArriveMessage() {
    return arriveMessage;
  }
  public void setArriveMessage(String arriveMessage) {
    this.arriveMessage = arriveMessage;
  }
  public String getLeaveMessage() {
    return leaveMessage;
  }
  public void setLeaveMessage(String leaveMessage) {
    this.leaveMessage = leaveMessage;
  }
  public int getMobFlags() {
    return mobFlags;
  }
  public void setMobFlags(int mobFlags) {
    this.mobFlags = mobFlags;
  }
  public int getAffectedFlags0() {
    return affectedFlags0;
  }
  public void setAffectedFlags0(int affectedFlags0) {
    this.affectedFlags0 = affectedFlags0;
  }
  public int getAffectedFlags1() {
    return affectedFlags1;
  }
  public void setAffectedFlags1(int affectedFlags1) {
    this.affectedFlags1 = affectedFlags1;
  }
  public int getAffectedFlags2() {
    return affectedFlags2;
  }
  public void setAffectedFlags2(int affectedFlags2) {
    this.affectedFlags2 = affectedFlags2;
  }
  public int getAffectedFlags3() {
    return affectedFlags3;
  }
  public void setAffectedFlags3(int affectedFlags3) {
    this.affectedFlags3 = affectedFlags3;
  }
  public int getLevel() {
    return level;
  }
  public void setLevel(int level) {
    this.level = level;
  }
  public int getMaxHit() {
    return maxHit;
  }
  public void setMaxHit(int maxHit) {
    this.maxHit = maxHit;
  }
  public int getMaxMana() {
    return maxMana;
  }
  public void setMaxMana(int maxMana) {
    this.maxMana = maxMana;
  }
  public int getNumberOfDamageDice() {
    return numberOfDamageDice;
  }
  public void setNumberOfDamageDice(int numberOfDamageDice) {
    this.numberOfDamageDice = numberOfDamageDice;
  }
  public int getDamageDiceSize() {
    return damageDiceSize;
  }
  public void setDamageDiceSize(int damageDiceSize) {
    this.damageDiceSize = damageDiceSize;
  }
  public int getNumerOfHitPointDice() {
    return numerOfHitPointDice;
  }
  public void setNumerOfHitPointDice(int numerOfHitPointDice) {
    this.numerOfHitPointDice = numerOfHitPointDice;
  }
  public int getHitPointDiceSize() {
    return hitPointDiceSize;
  }
  public void setHitPointDiceSize(int hitPointDiceSize) {
    this.hitPointDiceSize = hitPointDiceSize;
  }
  public int getDamageRoll() {
    return damageRoll;
  }
  public void setDamageRoll(int damageRoll) {
    this.damageRoll = damageRoll;
  }
  public int getExperience() {
    return experience;
  }
  public void setExperience(int experience) {
    this.experience = experience;
  }
  public int getPosition() {
    return position;
  }
  public void setPosition(int position) {
    this.position = position;
  }
  public int getDefaultPosition() {
    return defaultPosition;
  }
  public void setDefaultPosition(int defaultPosition) {
    this.defaultPosition = defaultPosition;
  }
  public int getSex() {
    return sex;
  }
  public void setSex(int sex) {
    this.sex = sex;
  }
  public int getRace() {
    return race;
  }
  public void setRace(int race) {
    this.race = race;
  }
  public int getOffensiveBonus() {
    return offensiveBonus;
  }
  public void setOffensiveBonus(int offensiveBonus) {
    this.offensiveBonus = offensiveBonus;
  }
  public int getParryBonus() {
    return parryBonus;
  }
  public void setParryBonus(int parryBonus) {
    this.parryBonus = parryBonus;
  }
  public int getDodgeBonus() {
    return dodgeBonus;
  }
  public void setDodgeBonus(int dodgeBonus) {
    this.dodgeBonus = dodgeBonus;
  }
  public int getClanId() {
    return clanId;
  }
  public void setClanId(int clanId) {
    this.clanId = clanId;
  }
  public int getRank() {
    return rank;
  }
  public void setRank(int rank) {
    this.rank = rank;
  }
  public int getMobClass() {
    return mobClass;
  }
  public void setMobClass(int mobClass) {
    this.mobClass = mobClass;
  }
  public int getPrimaryKit() {
    return primaryKit;
  }
  public void setPrimaryKit(int primaryKit) {
    this.primaryKit = primaryKit;
  }
  public int getFoodObjectId() {
    return foodObjectId;
  }
  public void setFoodObjectId(int foodObjectId) {
    this.foodObjectId = foodObjectId;
  }
  public int getFoodQuantity() {
    return foodQuantity;
  }
  public void setFoodQuantity(int foodQuantity) {
    this.foodQuantity = foodQuantity;
  }
  public int getSkinObjectId() {
    return skinObjectId;
  }
  public void setSkinObjectId(int skinObjectId) {
    this.skinObjectId = skinObjectId;
  }
  public int getSkinRequired() {
    return skinRequired;
  }
  public void setSkinRequired(int skinRequired) {
    this.skinRequired = skinRequired;
  }
  public int getSkinSkinned() {
    return skinSkinned;
  }
  public void setSkinSkinned(int skinSkinned) {
    this.skinSkinned = skinSkinned;
  }
  public int getWarrants0() {
    return warrants0;
  }
  public void setWarrants0(int warrants0) {
    this.warrants0 = warrants0;
  }
  public int getWarrants1() {
    return warrants1;
  }
  public void setWarrants1(int warrants1) {
    this.warrants1 = warrants1;
  }
  public int getWarrants2() {
    return warrants2;
  }
  public void setWarrants2(int warrants2) {
    this.warrants2 = warrants2;
  }
  public int getWarrants3() {
    return warrants3;
  }
  public void setWarrants3(int warrants3) {
    this.warrants3 = warrants3;
  }
  public int getGold() {
    return gold;
  }
  public void setGold(int gold) {
    this.gold = gold;
  }
  public int getAggro() {
    return aggro;
  }
  public void setAggro(int aggro) {
    this.aggro = aggro;
  }
  public int getNsects() {
    return nsects;
  }
  public void setNsects(int nsects) {
    this.nsects = nsects;
  }
  public int getAttack() {
    return attack;
  }
  public void setAttack(int attack) {
    this.attack = attack;
  }
  public int getStrength() {
    return strength;
  }
  public void setStrength(int strength) {
    this.strength = strength;
  }
  public int getDexterity() {
    return dexterity;
  }
  public void setDexterity(int dexterity) {
    this.dexterity = dexterity;
  }
  public int getIntelligence() {
    return intelligence;
  }
  public void setIntelligence(int intelligence) {
    this.intelligence = intelligence;
  }
  public int getWisdom() {
    return wisdom;
  }
  public void setWisdom(int wisdom) {
    this.wisdom = wisdom;
  }
  public int getConstitution() {
    return constitution;
  }
  public void setConstitution(int constitution) {
    this.constitution = constitution;
  }
  public String getAssists() {
    return assists;
  }
  public void setAssists(String assists) {
    this.assists = assists;
  }
}
