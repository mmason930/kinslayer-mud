package org.kinslayermud.mob;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.LinkedList;
import java.util.List;

import org.kinslayermud.exception.NonExistentObjectException;

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
               + " FROM mob_prototypes"
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
  
  public static List<MobPrototype> getSuperMobPrototypes(Statement statement) throws SQLException {
    
    String criteria = "(mob_flags & (1 << 23))";
    String orderBy = "sdesc";
    
    return getMobPrototypesMeetingCriteria(statement, criteria, orderBy);
  }
  
  public static MobPrototype getMobPrototype(ResultSet resultSet) throws SQLException {
    
    MobPrototype mobPrototype = new MobPrototype();
    
    //TODO: Load all fields.
    mobPrototype.setId(resultSet.getInt("vnum"));
    mobPrototype.setShortDescription(resultSet.getString("sdesc"));
    mobPrototype.setPrimaryKit(resultSet.getInt("primary_kit"));
    
    return mobPrototype;
  }
  
}
