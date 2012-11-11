package org.kinslayermud.object;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import org.kinslayermud.util.SQLUtil;

public class ObjectUtil {

  public static ObjectPrototype getObjectPrototype(ResultSet resultSet) throws SQLException {
    
    ObjectPrototype objectPrototype = new ObjectPrototype();
    
    objectPrototype.setId(resultSet.getInt("vnum"));
    objectPrototype.setShortDescription(resultSet.getString("sdesc"));
    objectPrototype.setDodgeBonus(resultSet.getInt("dodge"));
    objectPrototype.setParryBonus(resultSet.getInt("parry"));
    objectPrototype.setOffensiveBonus(resultSet.getInt("offensive"));
    objectPrototype.setAbsorb(resultSet.getInt("absorb"));
    objectPrototype.setWeight(resultSet.getBigDecimal("weight"));
    
    return objectPrototype;
  }
  
  public static Map<Integer, ObjectPrototype> getIdToObjectPrototypeMap(Statement statement, Collection<Integer> objectPrototypeIdCollection) throws SQLException {
    
    Map<Integer, ObjectPrototype> idToObjectPrototypeMap = new HashMap<Integer, ObjectPrototype>();
    
    String sql = " SELECT *"
               + " FROM obj_protos"
               + " WHERE vnum IN" + SQLUtil.buildListSQL(objectPrototypeIdCollection, false, true);
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      ObjectPrototype objectPrototype = getObjectPrototype(resultSet);
      idToObjectPrototypeMap.put(objectPrototype.getId(), objectPrototype);
    }
    
    return idToObjectPrototypeMap;
  }
  
}
