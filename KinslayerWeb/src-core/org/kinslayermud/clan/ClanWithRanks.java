package org.kinslayermud.clan;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class ClanWithRanks {

  protected Clan clan;
  protected List<ClanRank> clanRanks;
  protected Map<Integer, ClanRank> clanRankMap;
  
  public ClanWithRanks() {
    
    setClanRanks(new ArrayList<ClanRank>());
    clanRankMap = new HashMap<Integer, ClanRank>();
  }
  
  public Clan getClan() {
    
    return clan;
  }
  
  public void setClan(Clan clan) {
    
    this.clan = clan;
  }
  
  public List<ClanRank> getClanRanks() {
    
    return clanRanks;
  }
  
  public void setClanRanks(List<ClanRank> clanRanks) {
    
    this.clanRanks = clanRanks;
  }
  
  public void addClanRank(ClanRank clanRank) {
    
    clanRanks.add(clanRank);
    clanRankMap.put(clanRank.getRankNumber(), clanRank);
  }
  
  public ClanRank getClanRank(int rankNumber) {
    
    return clanRankMap.get(rankNumber);
  }
}
