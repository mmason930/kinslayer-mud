package org.kinslayermud.util;

import java.sql.SQLException;
import java.sql.Statement;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.kinslayermud.clan.Clan;
import org.kinslayermud.clan.ClanRank;
import org.kinslayermud.clan.ClanWithRanks;

public class ClanUtil {
  
  public static Map<Integer, Clan> getClanMap(Statement statement, Collection<Integer> clanIdCollection) throws SQLException {
    String criteria = " clan.id IN " + SQLUtil.buildListSQL(clanIdCollection, false, true);
    Collection<Clan> clans = QueryUtil.retrieveDataObjectList(statement, "clan", criteria, Clan.class);
    return MiscUtil.map(clans, clan -> clan.getId());
  }
  
  public static List<ClanRank> getClanRanksByClanIds(Statement statement, Collection<Integer> clanIdCollection) throws SQLException {
    
    String criteria = " clanRank.clan_id IN " + SQLUtil.buildListSQL(clanIdCollection, false, true);
    return QueryUtil.retrieveDataObjectList(statement, "clanRank", criteria, ClanRank.class);
  }
  
  public static Map<Integer, ClanWithRanks> getClanIdToClanWithRanksMap(Statement statement, Collection<Integer> clanIdCollection) throws SQLException {
    
    Map<Integer, Clan> clanMap = getClanMap(statement, clanIdCollection);
    List<ClanRank> clanRanks = getClanRanksByClanIds(statement, clanIdCollection);
    
    Map<Integer, ClanWithRanks> clanWithRanksMap = new HashMap<Integer, ClanWithRanks>();
    
    for(int clanId : clanMap.keySet()) {
      
      Clan clan = clanMap.get(clanId);
      
      ClanWithRanks clanWithRanks = new ClanWithRanks();
      
      clanWithRanks.setClan(clan);
      
      clanWithRanksMap.put(clanId, clanWithRanks);
    }
    
    for(ClanRank clanRank : clanRanks) {
      
      ClanWithRanks clanWithRanks = clanWithRanksMap.get(clanRank.getClanId());
      
      clanWithRanks.addClanRank(clanRank);
    }
    
    
    return clanWithRanksMap;
  }
}
