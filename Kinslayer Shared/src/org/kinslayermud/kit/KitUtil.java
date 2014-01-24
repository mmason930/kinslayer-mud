package org.kinslayermud.kit;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Map;

import org.kinslayermud.exception.NonExistentObjectException;
import org.kinslayermud.object.ObjectPrototype;
import org.kinslayermud.object.ObjectUtil;

public class KitUtil {

  public static Kit getKit(Statement statement, int kitId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT *"
               + " FROM kits"
               + " WHERE vnum = " + kitId;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getKit(resultSet);
    }
    
    if(throwIfNotFound) {
      
      throw new NonExistentObjectException("No kit found. ID = " + kitId);
    }
    
    return null;
  }
  
  public static Kit getKit(ResultSet resultSet) throws SQLException {
    
    Kit kit = new Kit();
    
    kit.setId(resultSet.getInt("vnum"));
    kit.setName(resultSet.getString("name"));
    
    return kit;
  }
  
  public static List<KitItem> getKitItemsMeetingCriteria(Statement statement, String criteria, String orderBy) throws SQLException {
    
    List<KitItem> kitItems = new ArrayList<KitItem>();
    
    String sql = " SELECT *"
               + " FROM kitItem"
               + " WHERE " + criteria
               + (orderBy != null ? " ORDER BY " + orderBy : "");
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      KitItem kitItem = getKitItem(resultSet);
      kitItems.add(kitItem);
    }
    
    return kitItems;
  }
  
  public static KitItem getKitItem(ResultSet resultSet) throws SQLException {
    
    KitItem kitItem = new KitItem();
    
    kitItem.setId(resultSet.getInt("id"));
    kitItem.setKitId(resultSet.getInt("kit_vnum"));
    kitItem.setBodyPosition(resultSet.getInt("body_position"));
    kitItem.setObjectPrototypeId(resultSet.getInt("obj_vnum"));
    kitItem.setProbability(resultSet.getInt("probability"));
    kitItem.setHierarchy(resultSet.getInt("hierarchy"));

    return kitItem;
  }
  
  public static KitWithItems getKitWithItems(Statement statement, int kitId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    Kit kit = getKit(statement, kitId, throwIfNotFound);
    KitWithItems kitWithItems = new KitWithItems();
    
    String criteria = "kit_vnum = " + kitId;
    String orderBy = "hierarchy";
    
    List<KitItem> kitItems = getKitItemsMeetingCriteria(statement, criteria, orderBy);
    List< List<KitItem> > kitItemList = new ArrayList< List<KitItem> >();
    
    Integer currentHiararchy = null;
    for(KitItem kitItem : kitItems) {
      
      if(currentHiararchy == null || currentHiararchy != kitItem.getHierarchy()) {
        
        kitItemList.add(new ArrayList<KitItem>());
        
        currentHiararchy = kitItem.getHierarchy();
      }
      
      kitItemList.get(kitItem.getHierarchy()).add(kitItem);
    }
    
    kitWithItems.setKit(kit);
    kitWithItems.setKitItemLists(kitItemList);
    
    return kitWithItems;
  }
  
  public static KitWithItemsAndObjectPrototypes getKitWithItemsAndObjectPrototypes(Statement statement, int kitId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    KitWithItemsAndObjectPrototypes kitWithItemsAndObjectProtoypes = new KitWithItemsAndObjectPrototypes();
    KitWithItems kitWithItems = getKitWithItems(statement, kitId, throwIfNotFound);
    
    Collection<Integer> objectPrototypeIdCollection = new HashSet<Integer>();
    
    //Generate a collection of all the object prototypes we will need to load.
    for(List<KitItem> kitItemList : kitWithItems.getKitItemLists()) {
      
      for(KitItem kitItem : kitItemList) {
        
        objectPrototypeIdCollection.add(kitItem.getObjectPrototypeId());
      }
    }
    
    Map<Integer, ObjectPrototype> idToObjectPrototypeMap = ObjectUtil.getIdToObjectPrototypeMap(statement, objectPrototypeIdCollection);
    
    kitWithItemsAndObjectProtoypes.setKitWithItems(kitWithItems);
    kitWithItemsAndObjectProtoypes.setIdToObjectPrototypeMap(idToObjectPrototypeMap);
    
    return kitWithItemsAndObjectProtoypes;
  }
  
}
