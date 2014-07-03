package org.kinslayermud.character;

import java.util.Date;


public class User {

  protected int userId;
  protected String userName;
  protected int level;
  protected Date lastLogout;
  protected Date lastLogon;
  protected UserClass userClass;
  protected UserRace userRace;
  
  protected String userPassword;
  protected String poofin;
  protected String poofout;
  protected int maxHit;
  protected int hit;
  protected int move;
  protected int maxMove;
  protected int mana;
  protected int maxMana;
  protected int shadow;
  protected int maxShadow;
  protected int weight;
  protected int height;
  protected Gender gender;
  protected int weave;
  protected int invis;
  protected int taint;
  protected int deathWait;
  protected int warnings;
  protected int masterWeapon;
  protected int strain;
  protected String slew;
  protected String whoisExtra;
  protected int gold;
  protected int bankGold;
  protected Date createdDatetime;
  protected int strength;
  protected int dexterity;
  protected int intelligence;
  protected int wisdom;
  protected int constitution;
  protected int exp;
  protected int featPoints;
  protected int played;
  protected Date restatDatetime;
  protected int pkDeaths;
  protected int mobDeaths;
  protected int pks;
  protected int mount;
  protected int loadRoom;
  protected int practices;
  protected int spellPractices;
  protected int tradePractices;
  protected int freeze;
  protected int badPasswords;
  protected int hunger;
  protected int thirst;
  protected int drunk;
  protected Date resetDatetime;
  protected String prf;
  protected int plr;
  protected int newb;
  protected String warrants;
  protected String title;
  protected String description;
  protected int accountId;
  
  public User() {
    
  }

  public int getUserId() {
    
    return userId;
  }
  
  public void setUserId(int userId) {
    
    this.userId = userId;
  }
  
  public String getUserName() {
    
    return userName;
  }
  
  public void setUserName(String userName) {
    
    this.userName = userName;
  }
  
  public int getLevel() {
    
    return level;
  }
  
  public void setLevel(int level) {
    
    this.level = level;
  }

  public Date getLastLogout() {
    return lastLogout;
  }

  public void setLastLogout(Date lastLogout) {
    this.lastLogout = lastLogout;
  }

  public Date getLastLogon() {
    return lastLogon;
  }

  public void setLastLogon(Date lastLogon) {
    this.lastLogon = lastLogon;
  }

  public UserClass getUserClass() {
    return userClass;
  }

  public void setUserClass(UserClass userClass) {
    this.userClass = userClass;
  }

  public UserRace getUserRace() {
    return userRace;
  }

  public void setUserRace(UserRace userRace) {
    this.userRace = userRace;
  }

  public String getUserPassword() {
    return userPassword;
  }

  public void setUserPassword(String userPassword) {
    this.userPassword = userPassword;
  }

  public String getPoofin() {
    return poofin;
  }

  public void setPoofin(String poofin) {
    this.poofin = poofin;
  }

  public String getPoofout() {
    return poofout;
  }

  public void setPoofout(String poofout) {
    this.poofout = poofout;
  }

  public int getMaxHit() {
    return maxHit;
  }

  public void setMaxHit(int maxHit) {
    this.maxHit = maxHit;
  }

  public int getHit() {
    return hit;
  }
  
  public void setHit(int hit) {
    this.hit = hit;
  }
  
  public int getMove() {
    return move;
  }

  public void setMove(int move) {
    this.move = move;
  }

  public int getMaxMove() {
    return maxMove;
  }

  public void setMaxMove(int maxMove) {
    this.maxMove = maxMove;
  }

  public int getMana() {
    return mana;
  }

  public void setMana(int mana) {
    this.mana = mana;
  }

  public int getMaxMana() {
    return maxMana;
  }

  public void setMaxMana(int maxMana) {
    this.maxMana = maxMana;
  }

  public int getShadow() {
    return shadow;
  }

  public void setShadow(int shadow) {
    this.shadow = shadow;
  }

  public int getMaxShadow() {
    return maxShadow;
  }

  public void setMaxShadow(int maxShadow) {
    this.maxShadow = maxShadow;
  }

  public int getWeight() {
    return weight;
  }

  public void setWeight(int weight) {
    this.weight = weight;
  }

  public int getHeight() {
    return height;
  }

  public void setHeight(int height) {
    this.height = height;
  }

  public Gender getGender() {
    return gender;
  }

  public void setGender(Gender gender) {
    this.gender = gender;
  }

  public int getWeave() {
    return weave;
  }

  public void setWeave(int weave) {
    this.weave = weave;
  }

  public int getInvis() {
    return invis;
  }

  public void setInvis(int invis) {
    this.invis = invis;
  }

  public int getTaint() {
    return taint;
  }

  public void setTaint(int taint) {
    this.taint = taint;
  }

  public int getDeathWait() {
    return deathWait;
  }

  public void setDeathWait(int deathWait) {
    this.deathWait = deathWait;
  }

  public int getWarnings() {
    return warnings;
  }

  public void setWarnings(int warnings) {
    this.warnings = warnings;
  }

  public int getMasterWeapon() {
    return masterWeapon;
  }

  public void setMasterWeapon(int masterWeapon) {
    this.masterWeapon = masterWeapon;
  }

  public int getStrain() {
    return strain;
  }

  public void setStrain(int strain) {
    this.strain = strain;
  }

  public String getSlew() {
    return slew;
  }

  public void setSlew(String slew) {
    this.slew = slew;
  }

  public String getWhoisExtra() {
    return whoisExtra;
  }

  public void setWhoisExtra(String whoisExtra) {
    this.whoisExtra = whoisExtra;
  }

  public int getGold() {
    return gold;
  }

  public void setGold(int gold) {
    this.gold = gold;
  }

  public int getBankGold() {
    return bankGold;
  }

  public void setBankGold(int bankGold) {
    this.bankGold = bankGold;
  }

  public Date getCreatedDatetime() {
    return createdDatetime;
  }

  public void setCreatedDatetime(Date createdDatetime) {
    this.createdDatetime = createdDatetime;
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
  
  public int getExp() {
    return exp;
  }

  public void setExp(int exp) {
    this.exp = exp;
  }

  public int getFeatPoints() {
    return featPoints;
  }

  public void setFeatPoints(int featPoints) {
    this.featPoints = featPoints;
  }

  public int getPlayed() {
    return played;
  }

  public void setPlayed(int played) {
    this.played = played;
  }

  public Date getRestatDatetime() {
    return restatDatetime;
  }

  public void setRestatDatetime(Date restatDatetime) {
    this.restatDatetime = restatDatetime;
  }

  public int getPkDeaths() {
    return pkDeaths;
  }

  public void setPkDeaths(int pkDeaths) {
    this.pkDeaths = pkDeaths;
  }
  
  public int getPks() {
    return pks;
  }
  
  public void setPks(int pks) {
    this.pks = pks;
  }

  public int getMobDeaths() {
    return mobDeaths;
  }

  public void setMobDeaths(int mobDeaths) {
    this.mobDeaths = mobDeaths;
  }

  public int getMount() {
    return mount;
  }

  public void setMount(int mount) {
    this.mount = mount;
  }

  public int getLoadRoom() {
    return loadRoom;
  }

  public void setLoadRoom(int loadRoom) {
    this.loadRoom = loadRoom;
  }

  public int getPractices() {
    return practices;
  }

  public void setPractices(int practices) {
    this.practices = practices;
  }

  public int getSpellPractices() {
    return spellPractices;
  }

  public void setSpellPractices(int spellPractices) {
    this.spellPractices = spellPractices;
  }

  public int getTradePractices() {
    return tradePractices;
  }

  public void setTradePractices(int tradePractices) {
    this.tradePractices = tradePractices;
  }

  public int getFreeze() {
    return freeze;
  }

  public void setFreeze(int freeze) {
    this.freeze = freeze;
  }

  public int getBadPasswords() {
    return badPasswords;
  }

  public void setBadPasswords(int badPasswords) {
    this.badPasswords = badPasswords;
  }

  public int getHunger() {
    return hunger;
  }

  public void setHunger(int hunger) {
    this.hunger = hunger;
  }

  public int getThirst() {
    return thirst;
  }

  public void setThirst(int thirst) {
    this.thirst = thirst;
  }

  public int getDrunk() {
    return drunk;
  }

  public void setDrunk(int drunk) {
    this.drunk = drunk;
  }

  public Date getResetDatetime() {
    return resetDatetime;
  }

  public void setResetDatetime(Date resetDatetime) {
    this.resetDatetime = resetDatetime;
  }

  public String getPrf() {
    return prf;
  }

  public void setPrf(String prf) {
    this.prf = prf;
  }

  public int getPlr() {
    return plr;
  }

  public void setPlr(int plr) {
    this.plr = plr;
  }

  public int getNewb() {
    return newb;
  }

  public void setNewb(int newb) {
    this.newb = newb;
  }

  public String getWarrants() {
    return warrants;
  }

  public void setWarrants(String warrants) {
    this.warrants = warrants;
  }

  public String getTitle() {
    return title;
  }

  public void setTitle(String title) {
    this.title = title;
  }

  public String getDescription() {
    return description;
  }

  public void setDescription(String description) {
    this.description = description;
  }

  public int getAccountId() {
    return accountId;
  }

  public void setAccountId(int accountId) {
    this.accountId = accountId;
  }
  
  public boolean preferenceFlagged(UserPreference userPreference) {
    
    String[] preferences = getPrf().split(" ");
    
    int preferenceInt32 = Integer.valueOf(preferences[userPreference.value() / 32]);
    
    return (preferenceInt32 & (1 << (userPreference.value() % 32))) != 0;
  }
}
