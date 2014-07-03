package org.kinslayermud.clan;

import org.kinslayermud.misc.DataObjectWithIntId;

public class ClanRank extends DataObjectWithIntId {

  protected int clanId;
  protected int rankNumber;
  protected String name;
  
  public ClanRank() {
    
    id = NEW;
  }
  
  public int getClanId() {
    return clanId;
  }
  public void setClanId(int clanId) {
    this.clanId = clanId;
  }
  public int getRankNumber() {
    return rankNumber;
  }
  public void setRankNumber(int rankNumber) {
    this.rankNumber = rankNumber;
  }
  public String getName() {
    return name;
  }
  public void setName(String name) {
    this.name = name;
  }
}
