package org.kinslayermud.dbutils;

import java.sql.ResultSet;
import java.sql.SQLException;

public interface DBObject {
  
  public void retrieveFromResultSet(ResultSet resultSet) throws SQLException;
}
