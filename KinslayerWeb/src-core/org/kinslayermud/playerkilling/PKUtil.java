package org.kinslayermud.playerkilling;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.kinslayermud.util.QueryUtil;
import org.kinslayermud.util.SQLUtil;

public class PKUtil {
  
  public static Collection<Integer> getLastSoManyKillIds(Statement statement, int howMany) throws SQLException {
    
    String sql = " SELECT DISTINCT kill_id AS kill_id"
               + " FROM userPlayerKill"
               + " ORDER BY kill_id DESC"
               + " LIMIT " + howMany;
    Collection<Integer> killIdCollection = new HashSet<Integer>();
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      killIdCollection.add(resultSet.getInt("kill_id"));
    }
    
    return killIdCollection;
  }
  
  public static List<PlayerKill> getLastSoManyPlayerKills(Statement statement, int howMany) throws SQLException {
    
    List<UserPlayerKill> userPlayerKills;
    Collection<Integer> killIdCollection = getLastSoManyKillIds(statement, howMany);
    
    String criteria = "kill_id IN" + SQLUtil.buildListSQL(killIdCollection, false, true);
    String orderBy = "kill_id DESC";
    
    userPlayerKills = QueryUtil.retrieveDataObjectList(statement, "userPlayerKill", criteria, orderBy, UserPlayerKill.class);
    List<PlayerKill> playerKills = convertToPlayerKills(userPlayerKills);
    
    return playerKills;
  }
  
  public static List<PlayerKill> convertToPlayerKills(List<UserPlayerKill> userPlayerKills) {

    List<PlayerKill> playerKills = new LinkedList<PlayerKill>();
    Map<Integer, PlayerKill> playerKillMap = new HashMap<Integer, PlayerKill>();
    
    for(UserPlayerKill userPlayerKill : userPlayerKills) {
      
      PlayerKill playerKill;
      if(playerKillMap.containsKey(userPlayerKill.getKillId()) == false) {
        
        playerKill = new PlayerKill();
        playerKill.setKillId(userPlayerKill.getKillId());
        playerKill.setVictimUserId(userPlayerKill.getVictimId());
        playerKill.setTimeOfDeath(userPlayerKill.getTimeOfDeath());
        
        playerKillMap.put(userPlayerKill.getKillId(), playerKill);
        playerKills.add(playerKill);
      }
      else {
        
        playerKill = playerKillMap.get(userPlayerKill.getKillId());
      }
      
      playerKill.getUserPlayerKills().add(userPlayerKill);
    }
    
    return playerKills;
  }
  
  public static Collection<Integer> getPlayerKillIdsByKillerId(Statement statement, int killerUserId, Integer offset, Integer fetchSize, boolean sort) throws SQLException {
    
    String sql = " SELECT kill_id"
               + " FROM userPlayerKill"
               + " WHERE killer_id = " + killerUserId
               + (sort ? " ORDER BY time_of_death DESC" : "")
               + SQLUtil.buildLimit(offset, fetchSize);
    
    Collection<Integer> playerKillIds = new HashSet<Integer>();
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      playerKillIds.add(resultSet.getInt("kill_id"));
    }
    
    return playerKillIds;
  }
  
  public static List<PlayerKill> getPlayerKillsByKillerId(Statement statement, int killerUserId, Integer offset, Integer fetchSize, boolean sort) throws SQLException {
    
    Collection<Integer> playerKillIdCollection = getPlayerKillIdsByKillerId(statement, killerUserId, offset, fetchSize, sort);
    
    String criteria = "kill_id IN" + SQLUtil.buildListSQL(playerKillIdCollection, false, true);
    
    List<UserPlayerKill> userPlayerKills = QueryUtil.retrieveDataObjectList(statement, "userPlayerKill", criteria, UserPlayerKill.class);
    
    return convertToPlayerKills(userPlayerKills);
  }
}
