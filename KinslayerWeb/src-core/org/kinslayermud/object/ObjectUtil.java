package org.kinslayermud.object;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.kinslayermud.util.SQLUtil;
import org.kinslayermud.util.StringUtil;

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
    objectPrototype.setWear(resultSet.getInt("wear"));

    objectPrototype.setValue(0 , resultSet.getInt("obj_flag0"));
    objectPrototype.setValue(1 , resultSet.getInt("obj_flag1"));
    objectPrototype.setValue(2 , resultSet.getInt("obj_flag2"));
    objectPrototype.setValue(3 , resultSet.getInt("obj_flag3"));
    objectPrototype.setValue(4 , resultSet.getInt("obj_flag4"));
    objectPrototype.setValue(5 , resultSet.getInt("obj_flag5"));
    objectPrototype.setValue(6 , resultSet.getInt("obj_flag6"));
    objectPrototype.setValue(7 , resultSet.getInt("obj_flag7"));
    objectPrototype.setValue(8 , resultSet.getInt("obj_flag8"));
    objectPrototype.setValue(9 , resultSet.getInt("obj_flag9"));
    objectPrototype.setValue(10, resultSet.getInt("obj_flag10"));
    objectPrototype.setValue(11, resultSet.getInt("obj_flag11"));
    
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
  
  public static List<ObjectPrototype> getObjectPrototypesMeetingCriteria(Statement statement, String criteria, String orderBy) throws SQLException {
    
    List<ObjectPrototype> objectPrototypes = new LinkedList<ObjectPrototype>();
    String sql = " SELECT *"
               + " FROM obj_protos"
               + " WHERE " + criteria
               + (orderBy != null ? " ORDER BY " + orderBy : "");
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      objectPrototypes.add(getObjectPrototype(resultSet));
    }
    
    return objectPrototypes;
  }
  
  public static List<ObjectPrototype> getObjectPrototypesByWearType(Statement statement, ObjectWearType objectWearType) throws SQLException {
    
    String criteria = " wear & (1 << " + objectWearType + ")"
                    + " AND type_flag = " + ObjectType.armor
                    + " AND !(extra_flags &(1 << " + ObjectExtraFlag.noShow + "))";
    String orderBy = " sdesc";
    
    return getObjectPrototypesMeetingCriteria(statement, criteria, orderBy);
  }
  
  public static List<ObjectPrototype> getObjectPrototypesByWeaponType(Statement statement, ObjectWeaponType objectWeaponType) throws SQLException {
    
    String criteria = " obj_flag0 = " + objectWeaponType
                    + " AND type_flag = " + ObjectType.weapon
                    + " AND !(extra_flags &(1 << " + ObjectExtraFlag.noShow + "))";
    String orderBy = " sdesc";
    
    return getObjectPrototypesMeetingCriteria(statement, criteria, orderBy);
  }
  
  public static Obj getObject(ResultSet resultSet) throws SQLException {
    
    Obj object = new Obj();
    
    object.setId(resultSet.getString("objects.id"));
    object.setVnum(resultSet.getInt("objects.vnum"));
    
    String retoolShortDescription = resultSet.getString("object_retools.retool_sdesc");
    String specialShortDescription = resultSet.getString("object_specials.sdesc");
    String objectPrototypeShortDescription = resultSet.getString("obj_protos.sdesc");
    
    if(!StringUtil.isNullOrEmptyTrimmedString(retoolShortDescription)) {
      object.setShortDescription(retoolShortDescription);
    }
    else if(!StringUtil.isNullOrEmptyTrimmedString(specialShortDescription)) {
      object.setShortDescription(specialShortDescription);
    }
    else {
      object.setShortDescription(objectPrototypeShortDescription);
    }
    
    return object;
  }
  
  public static Map<String, Obj> getObjectMap(Statement statement, Set<String> objectIdSet) throws SQLException {
    
    Map<String, Obj> objectMap = new HashMap<String, Obj>();
    String sql = " SELECT "
               + "   objects.id,"
               + "   objects.vnum,"
               + "   obj_protos.sdesc,"
               + "   object_specials.sdesc,"
               + "   object_retools.retool_sdesc"
               + " FROM (objects)"
               + " LEFT JOIN obj_protos ON obj_protos.vnum = objects.vnum"
               + " LEFT JOIN object_specials ON object_specials.id = objects.id"
               + " LEFT JOIN object_retools ON object_retools.id = objects.id"
               + " WHERE objects.id IN " + SQLUtil.buildListSQL(objectIdSet, true, true);
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      Obj object = getObject(resultSet);
      
      objectMap.put(object.getId(), object);
    }
    
    return objectMap;
  }
}
