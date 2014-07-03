package org.kinslayermud.character;

import java.util.Date;

import org.kinslayermud.misc.DataObjectWithIntId;

public class UserClan extends DataObjectWithIntId {

  protected int userId;
  protected int clanId;
  protected Date clannedDatetime;
  protected Date lastRankedDatetime;
  protected int questPoints;
  protected int rank;
  protected boolean isCouncil;
  
  public UserClan() {
    
    id = NEW;
  }
  
  public int getUserId() {
    return userId;
  }
  public void setUserId(int userId) {
    this.userId = userId;
  }
  public int getClanId() {
    return clanId;
  }
  public void setClanId(int clanId) {
    this.clanId = clanId;
  }
  public Date getClannedDatetime() {
    return clannedDatetime;
  }
  public void setClannedDatetime(Date clannedDatetime) {
    this.clannedDatetime = clannedDatetime;
  }
  public Date getLastRankedDatetime() {
    return lastRankedDatetime;
  }
  public void setLastRankedDatetime(Date lastRankedDatetime) {
    this.lastRankedDatetime = lastRankedDatetime;
  }
  public int getQuestPoints() {
    return questPoints;
  }
  public void setQuestPoints(int questPoints) {
    this.questPoints = questPoints;
  }
  public int getRank() {
    return rank;
  }
  public void setRank(int rank) {
    this.rank = rank;
  }
  public boolean isCouncil() {
    return isCouncil;
  }
  public void setCouncil(boolean isCouncil) {
    this.isCouncil = isCouncil;
  }
}
