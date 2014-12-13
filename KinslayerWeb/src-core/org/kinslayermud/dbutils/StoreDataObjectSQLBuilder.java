package org.kinslayermud.dbutils;

import java.sql.SQLException;
import java.sql.Statement;

import org.apache.log4j.Logger;
import org.kinslayermud.misc.DataObjectWithIntId;
import org.kinslayermud.util.QueryUtil;

public class StoreDataObjectSQLBuilder extends StoreSQLBuilder {

  protected Statement statement;
  
  public StoreDataObjectSQLBuilder(Statement statement, String tableName) {
    
    super(tableName);
    this.statement = statement;
  }
  
  private static final Logger logger = Logger.getLogger(StoreDataObjectSQLBuilder.class.getName());
  
  public int execute(DataObjectWithIntId dataObject) throws SQLException {
    
    if(dataObject.isNew()) {
      
      statement.executeUpdate(generateInsert());
      
      dataObject.setId(QueryUtil.getLastInsertedID(statement));
    }
    else {
      
      statement.executeUpdate(generateUpdate(dataObject.getId()));
    }
    
    return dataObject.getId();
  }
  /***
  public long execute(DataObjectWithLongId dataObject) throws SQLException {
    
    if(dataObject.isNew()) {
      
      statement.executeUpdate(generateInsert());
      
      dataObject.setId(QueryUtil.getLastInsertedLongID(statement));
    }
    else {
      
      statement.executeUpdate(generateUpdate(dataObject.getId()));
    }
    
    return dataObject.getId();
  }
  ***/
}
