package org.kinslayermud.comm;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Calendar;
import java.util.Collection;
import java.util.Date;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;

import org.kinslayermud.util.DateUtil;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.SQLUtil;

public class CommUtil {

  public static Comm getComm(ResultSet resultSet) throws SQLException {
    
    Comm comm = new Comm();
    
    comm.setId(resultSet.getInt("id"));
    comm.setType(resultSet.getString("type"));
    comm.setMessage(resultSet.getString("message"));
    comm.setSenderType(resultSet.getString("sender_type").charAt(0));
    comm.setSenderUserId(resultSet.getInt("sender_id"));
    comm.setRecipientType(resultSet.getString("recipient_type").charAt(0));
    comm.setRecipientUserId(resultSet.getInt("recipient_id"));
    comm.setRoomVnum(resultSet.getInt("room_vnum"));
    comm.setInvisLevel(resultSet.getInt("invis_level"));
    comm.setTimestamp(DateUtil.getCalendar(resultSet.getLong("timestamp") * 1000).getTime());
    
    return comm;
  }
  
  public static List<Comm> getTellHistory(Statement statement, int userId, int offset, int fetchSize) throws SQLException {
    
    String sql;
    String tempTableName = "tempTellHistory" + userId;
    ResultSet resultSet;
    boolean recreateTempTable = false;
    
    sql = " CREATE TABLE IF NOT EXISTS " + tempTableName + "("
        + "   `id` int(11) unsigned not null auto_increment,"
        + "   `comm_id` int(11) unsigned not null,"
        + "   `expired_datetime` datetime not null,"
        + "   PRIMARY KEY(`id`)"
        + " ) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci";
    
    statement.executeUpdate(sql);
    
    sql = " SELECT expired_datetime"
        + " FROM " + tempTableName
        + " LIMIT 1";
    
    resultSet = statement.executeQuery(sql);
    
    if(!resultSet.next()) {
      
      recreateTempTable = true;
    }
    else {
      
      Date expiredDatetime = resultSet.getTimestamp("expired_datetime");
      
      if( System.currentTimeMillis() >= expiredDatetime.getTime() ) {
        
        recreateTempTable = true;
      }
    }
    
    if(recreateTempTable) {
      
      Calendar calendar = DateUtil.getCalendar(new Date());
      calendar.add(Calendar.MINUTE, 15);
      
      sql = " TRUNCATE TABLE " + tempTableName;
      statement.executeUpdate(sql);
      
      sql = " INSERT INTO " + tempTableName + "("
          + "   `comm_id`,"
          + "   `expired_datetime`"
          + " )"
          + " SELECT id, " + SQLUtil.encodeQuoteDate(calendar.getTime())
          + " FROM comm"
          + " WHERE type = 'tell'"
          + " AND"
          + " ("
          + "       (sender_type='C' AND sender_id = " + userId + ")"
          + "  OR   (recipient_type='C' AND recipient_id = " + userId + ")"
          + " )"
          + " ORDER BY id DESC";
      
      statement.executeUpdate(sql);
    }
    
    sql = " SELECT comm_id"
        + " FROM " + tempTableName
        + " WHERE id BETWEEN " + (offset + 1) + " AND " + (offset + fetchSize);
    
    resultSet = statement.executeQuery(sql);
    Collection<Integer> commIdCollection = new HashSet<Integer>();
    while(resultSet.next()) {
      
      commIdCollection.add(resultSet.getInt("comm_id"));
    }
    
    sql = " SELECT *"
        + " FROM comm"
        + " WHERE id IN" + SQLUtil.buildListSQL(commIdCollection, false, true);
    
    List<Comm> tellHistory = new LinkedList<Comm>();
    resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      Comm comm = getComm(resultSet);
      tellHistory.add(comm);
    }
    
    return tellHistory;
  }
}
