package org.kinslayermud.mob;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.kinslayermud.exception.NonExistentObjectException;
import org.kinslayermud.util.SQLUtil;

public class MobUtil {

  public static MobPrototype getMobPrototypeMeetingCriteria(Statement statement, String criteria, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT *"
               + " FROM mob_protos"
               + " WHERE " + criteria;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getMobPrototype(resultSet);
    }
    
    if(throwIfNotFound) {
      
      throw new NonExistentObjectException("No mob prototype found. Criteria: " + criteria);
    }
    
    return null;
  }
  
  public static MobPrototype getMobPrototype(Statement statement, int mobPrototypeId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String criteria = "vnum = " + mobPrototypeId;
    
    return getMobPrototypeMeetingCriteria(statement, criteria, throwIfNotFound);
  }
  
  public static List<MobPrototype> getMobPrototypesMeetingCriteria(Statement statement, String criteria, String orderBy) throws SQLException {
    
    String sql = " SELECT *"
               + " FROM mob_protos"
               + " WHERE " + criteria
               + (orderBy != null ? " ORDER BY " + orderBy : "");
    List<MobPrototype> mobPrototypes = new LinkedList<MobPrototype>();
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      MobPrototype mobPrototype = getMobPrototype(resultSet);
      
      mobPrototypes.add(mobPrototype);
    }
    
    return mobPrototypes;
  }
  
  public static Map<Integer, MobPrototype> getMobPrototypeMap(Statement statement, Collection<Integer> mobIdCollection) throws SQLException {
    
    String criteria = "vnum IN" + SQLUtil.buildListSQL(mobIdCollection, false, true);
    List<MobPrototype> mobPrototypes = getMobPrototypesMeetingCriteria(statement, criteria, null);
    Map<Integer, MobPrototype> mobPrototypeMap = new HashMap<Integer, MobPrototype>();
    
    for(MobPrototype mobPrototype : mobPrototypes) {
      
      mobPrototypeMap.put(mobPrototype.getId(), mobPrototype);
    }
    
    return mobPrototypeMap;
  }
  
  
  public static MobPrototype getMobPrototype(ResultSet resultSet) throws SQLException {
    
    MobPrototype mobPrototype = new MobPrototype();
    
    //TODO: Load all fields.
    mobPrototype.setId(resultSet.getInt("vnum"));
    mobPrototype.setShortDescription(resultSet.getString("sdesc"));
    mobPrototype.setPrimaryKit(resultSet.getInt("primary_kit"));
    
    return mobPrototype;
  }
  
  public static List<SuperMob> getSuperMobsMeetingCriteria(Statement statement, String criteria, String orderBy) throws SQLException {
    
    List<SuperMob> superMobs = new LinkedList<SuperMob>();
    
    String sql = " SELECT *"
               + " FROM superMob"
               + " WHERE " + criteria
               + (orderBy != null ? (" ORDER BY " + orderBy) : "");
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      SuperMob superMob = getSuperMob(resultSet);
      
      superMobs.add(superMob);
    }
    
    return superMobs;
  }
  
  public static List<SuperMob> getAllOpenSuperMobs(Statement statement) throws SQLException {
    
    String criteria = "status = " + SuperMobStatus.open;
    return getSuperMobsMeetingCriteria(statement, criteria, null);
  }
  
  public static SuperMob getSuperMob(ResultSet resultSet) throws SQLException {
    
    SuperMob superMob = new SuperMob();

    superMob.setId(resultSet.getInt("id"));
    superMob.setMobId(resultSet.getInt("mob_id"));
    superMob.setStatus(SuperMobStatus.getEnum(resultSet.getInt("status")));
    superMob.setDifficulty(SuperMobDifficulty.getEnum(resultSet.getInt("difficulty")));
    superMob.setDescription(resultSet.getString("description"));
    superMob.setMobImageUrl(resultSet.getString("mob_image_url"));
    superMob.setMapImageUrl(resultSet.getString("map_image_url"));
    
    return superMob;
  }
}
