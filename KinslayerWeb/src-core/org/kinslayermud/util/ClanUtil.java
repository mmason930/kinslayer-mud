package org.kinslayermud.util;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.kinslayermud.clan.Clan;
import org.kinslayermud.clan.ClanRank;
import org.kinslayermud.clan.ClanWithRanks;

public class ClanUtil {

  public static Clan getClan(ResultSet resultSet) throws SQLException {
    
    Clan clan = new Clan();
    
    clan.setId(resultSet.getInt("clan.id"));
    clan.setName(resultSet.getString("clan.name"));
    clan.setSecret(QueryUtil.getIntBoolean(resultSet, "clan.is_secret"));
    clan.setWarrantId(resultSet.getInt("clan.warrant_id"));
    clan.setHiddenLevel(resultSet.getInt("clan.hidden_level"));
    clan.setBonusSkillId(resultSet.getInt("clan.bonus_skill_id"));
    clan.setBonusSkillModifier(resultSet.getInt("clan.bonus_skill_modifier"));
    clan.setBonusSkillRank(resultSet.getInt("clan.bonus_skill_rank"));
    
    return clan;
  }
  
  public static ClanRank getClanRank(ResultSet resultSet) throws SQLException {
    
    ClanRank clanRank = new ClanRank();
    
    clanRank.setId(resultSet.getInt("clanRank.id"));
    clanRank.setClanId(resultSet.getInt("clanRank.clan_id"));
    clanRank.setRankNumber(resultSet.getInt("clanRank.rank_number"));
    clanRank.setName(resultSet.getString("clanRank.name"));
    
    return clanRank;
  }
  
  public static List<Clan> getClansMeetingCriteria(Statement statement, String criteria) throws SQLException {
    
    List<Clan> clans = new ArrayList<Clan>();
    
    String sql = " SELECT *"
               + " FROM clan"
               + " WHERE " + criteria;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      Clan clan = getClan(resultSet);
      
      clans.add(clan);
    }
    
    resultSet.close();
    
    return clans;
  }
  
  public static Map<Integer, Clan> getClanMap(List<Clan> clans) {
    
    Map<Integer, Clan> clanMap = new HashMap<Integer, Clan>();
    
    for(Clan clan : clans) {
      
      clanMap.put(clan.getId(), clan);
    }
    
    return clanMap;
  }
  
  public static Map<Integer, Clan> getClanMap(Statement statement, Collection<Integer> clanIdCollection) throws SQLException {
    
    String criteria = " clan.id IN " + SQLUtil.buildListSQL(clanIdCollection, false, true);
    return getClanMap(getClansMeetingCriteria(statement, criteria));
  }
  
  public static List<ClanRank> getClanRanksMeetingCriteria(Statement statement, String criteria) throws SQLException {
    
    List<ClanRank> clanRanks = new ArrayList<ClanRank>();
    
    String sql = " SELECT *"
               + " FROM clanRank"
               + " WHERE " + criteria;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      ClanRank clanRank = getClanRank(resultSet);
      
      clanRanks.add(clanRank);
    }
    
    resultSet.close();
    
    return clanRanks;
  }
  
  public static List<ClanRank> getClanRanksByClanIds(Statement statement, Collection<Integer> clanIdCollection) throws SQLException {
    
    String criteria = " clanRank.clan_id IN " + SQLUtil.buildListSQL(clanIdCollection, false, true);
    return getClanRanksMeetingCriteria(statement, criteria);
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
