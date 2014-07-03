package org.kinslayermud.playerkilling;

import java.util.Collection;
import java.util.Date;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;

public class PlayerKill {

  protected int killId;
  protected int victimUserId;
  protected Date timeOfDeath;
  protected List<UserPlayerKill> userPlayerKills = new LinkedList<UserPlayerKill>();
  
  public int getKillId() {
    return killId;
  }
  public void setKillId(int killId) {
    this.killId = killId;
  }
  public Date getTimeOfDeath() {
    return timeOfDeath;
  }
  public void setTimeOfDeath(Date timeOfDeath) {
    this.timeOfDeath = timeOfDeath;
  }
  public List<UserPlayerKill> getUserPlayerKills() {
    
    return userPlayerKills;
  }
  public void setUserPlayerKills(List<UserPlayerKill> userPlayerKills) {
    
    this.userPlayerKills = userPlayerKills;
  }
  public int getVictimUserId() {
    
    return victimUserId;
  }
  public void setVictimUserId(int victimUserId) {
    
    this.victimUserId = victimUserId;
  }
  
  public Collection<Integer> getUserIdSet() {
    
    Set<Integer> userIdSet = new HashSet<Integer>();
    
    userIdSet.add(victimUserId);
    
    for(UserPlayerKill userPlayerKill : userPlayerKills) {
      
      userIdSet.add(userPlayerKill.getKillerId());
    }
    
    return userIdSet;
  }
  
  public Collection<Integer> getKillerUserIdSet() {
    
    Set<Integer> userIdSet = new HashSet<Integer>();
    
    for(UserPlayerKill userPlayerKill : userPlayerKills) {
      
      userIdSet.add(userPlayerKill.getKillerId());
    }
    
    return userIdSet;
  }

  public int getTotalWeavePointTransfer() {
    
    int totalWeavePointTransfer = 0;
    for(UserPlayerKill userPlayerKill : userPlayerKills) {
      
      totalWeavePointTransfer += userPlayerKill.getWeavePointTransfer();
    }
    
    return totalWeavePointTransfer;
  }
  
  public UserPlayerKill getUserPlayerKillByKillerUserId(int killerUserId) {
    
    for(UserPlayerKill userPlayerKill : userPlayerKills) {
      
      if(userPlayerKill.getKillerId() == killerUserId) {
        
        return userPlayerKill;
      }
    }
    
    return null;
  }
}
