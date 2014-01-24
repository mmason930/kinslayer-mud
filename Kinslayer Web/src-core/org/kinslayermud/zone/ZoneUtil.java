package org.kinslayermud.zone;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import org.kinslayermud.exception.NonExistentObjectException;
import org.kinslayermud.util.QueryUtil;
import org.kinslayermud.util.SQLUtil;

public class ZoneUtil {

  public static Zone getZoneLoadingSuperMobPrototype(Statement statement, int mobPrototypeId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT zoneIndex.*"
               + " FROM zoneIndex, zoneCommand"
               + " WHERE zoneIndex.id = zoneCommand.zone_id"
               + " AND zoneCommand.arg1 = " + mobPrototypeId
               + " AND zoneCommand.cmd = 'M'"
               + " AND zoneIndex.closed = " + SQLUtil.encodeBooleanInt(false);
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getZone(resultSet);
    }
    
    if(throwIfNotFound) {
      
      throw new NonExistentObjectException("No open zone found loading mob prototype #" + mobPrototypeId + ".");
    }
    
    return null;
  }
  
  public static Zone getZone(ResultSet resultSet) throws SQLException {
    
    Zone zone = new Zone();
    
    zone.setId(resultSet.getInt("id"));
    zone.setName(resultSet.getString("name"));
    zone.setLifespan(resultSet.getInt("lifespan"));
    zone.setX(resultSet.getInt("x"));
    zone.setY(resultSet.getInt("y"));
    zone.setBottomRoomId(resultSet.getInt("bot"));
    zone.setTopRoomId(resultSet.getInt("top"));
    zone.setResetMode(resultSet.getInt("reset_mode"));
    zone.setSunrise(resultSet.getString("sunrise"));
    zone.setSunset(resultSet.getString("sunset"));
    zone.setBuilders(resultSet.getString("builders"));
    zone.setClosed(QueryUtil.getIntBoolean(resultSet, "closed"));
    
    return zone;
  }
  
}
