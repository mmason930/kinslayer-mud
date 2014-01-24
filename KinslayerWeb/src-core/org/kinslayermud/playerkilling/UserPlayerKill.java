package org.kinslayermud.playerkilling;

import java.util.Date;

import org.kinslayermud.misc.DataObjectWithIntId;

public class UserPlayerKill extends DataObjectWithIntId {

  protected int killId;
  protected int killerId;
  protected int victimId;
  protected int weavePointTransfer;
  protected Date timeOfDeath;

  public int getKillId() {
    return killId;
  }
  public void setKillId(int killId) {
    this.killId = killId;
  }
  public int getKillerId() {
    return killerId;
  }
  public void setKillerId(int killerId) {
    this.killerId = killerId;
  }
  public int getVictimId() {
    return victimId;
  }
  public void setVictimId(int victimId) {
    this.victimId = victimId;
  }
  public int getWeavePointTransfer() {
    return weavePointTransfer;
  }
  public void setWeavePointTransfer(int weavePointTransfer) {
    this.weavePointTransfer = weavePointTransfer;
  }
  public Date getTimeOfDeath() {
    return timeOfDeath;
  }
  public void setTimeOfDeath(Date timeOfDeath) {
    this.timeOfDeath = timeOfDeath;
  }
  
}
