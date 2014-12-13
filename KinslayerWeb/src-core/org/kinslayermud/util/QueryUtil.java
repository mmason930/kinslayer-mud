package org.kinslayermud.util;

import java.math.BigDecimal;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.kinslayermud.dbutils.DBObject;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;
import org.kinslayermud.genericaccessor.GenericAccessor;

/** Utility methods for working with database queries. */
public abstract class QueryUtil {

  /**
   * Closes a connection hiding any SQLException thrown.  Use with caution.
   * Null connections are ignored.
   */
  public static void closeNoThrow (Connection connection) {
    if (connection == null)
      return;

    try {
      connection.close();
    }
    catch (SQLException sqle) {
      // Ignore - as requested
    }
  }

  /**
   * Rolls-back a connection hiding any SQLException thrown.  Use with caution.
   * If an exception is thrown, the method tries to call closeNoThrow on the connection.  
   * Null connections are ignored.
   */
  public static void rollbackNoThrow (Connection connection) {
    if (connection == null)
      return;

    try {
      connection.rollback();
    }
    catch (SQLException sqle) {
      // Try to close the connection and then ignore - as requested
      closeNoThrow(connection);
    }
  }

  /**
   * Closes a statement hiding any SQLException thrown.  Use with extreme caution.
   * Null statement are ignored.
   */
  public static void closeNoThrow (Statement statement) {
    if (statement == null)
      return;

    try {
      statement.close();
    }
    catch (SQLException sqle) {
      // Ignore - as requested
    }
  }

  /**
   * Execute the specified update statement on the specified connection.  This
   * method creates a statement, executes the sql using that statement, then
   * closes the statement.<br>
   * <br>
   * NOTE: This should not be used in situations where multiple SQL calls could
   * be made with a single Statement instance.
   */
  public static int executeUpdate (Connection connection, String sql) throws SQLException {
    Statement statement = connection.createStatement();
    int result = statement.executeUpdate(sql);
    statement.close();
    return result;
  }

  /**
   * Used to check the results of an existance-query that depends on zero or
   * one rows being present in a result-set.  Generally used with queries
   * like 'SELECT 1 FROM table WHERE id = value' to see if the row with an
   * id that matches the specified value exists.<br>
   * <br>
   * NOTE: This method throws an exception if more than one rows exist.
   */
  public static boolean hasSingleResultRow (Statement statement, String sql) throws SQLException {
    ResultSet rs = statement.executeQuery(sql);

    boolean hasResultRow = rs.next();

    if (rs.next())
      throw new SQLException("Invalid number of rows: multiple");

    return hasResultRow;
  }

  /**
   * Used to check the results of an existance-query that depends on zero or
   * one or more rows being present in a result-set.  Generally used with queries
   * like 'SELECT 1 FROM table WHERE id = value' to see if the row with an
   * id that matches the specified value exists.<br>
   */
  public static boolean hasAtLeastOneRow (Statement statement, String sql) throws SQLException {
    ResultSet rs = statement.executeQuery(sql);

    boolean hasResultRow = rs.next();

    return hasResultRow;
  }

  /**
   * Performs the specified query and returns the result.  The query should
   * produce a single row containing a single integer value.  If the value
   * is null, zero will be returned.
   */
  public static int getSingleIntValueResult (Statement statement, String sql) throws SQLException {
    ResultSet rs = statement.executeQuery(sql);

    if (!rs.next())
      throw new SQLException("Invalid number of rows: none");

    int value = rs.getInt(1);

    if (rs.next())
      throw new SQLException("Invalid number of rows: multiple");

    rs.close();

    return value;
  }

  /**
   * Performs the specified query and returns the result.  The query should
   * produce a single row containing a single long integer value.  If the value
   * is null, zero will be returned.
   */
  public static long getSingleLongValueResult (Statement statement, String sql) throws SQLException {
    ResultSet rs = statement.executeQuery(sql);

    if (!rs.next())
      throw new SQLException("Invalid number of rows: none");

    long value = rs.getLong(1);

    if (rs.next())
      throw new SQLException("Invalid number of rows: multiple");

    rs.close();

    return value;
  }

  /**
   * Performs the specified query and returns the result.  The query should
   * produce a single row containing a single integer-boolean value.  If the
   * value is null, false will be returned.
   */
  public static boolean getSingleIntBooleanValueResult (Statement statement, String sql) throws SQLException {
    ResultSet rs = statement.executeQuery(sql);

    if (!rs.next())
      throw new SQLException("Invalid number of rows: none");

    boolean value = getIntBoolean(rs, 1);

    if (rs.next())
      throw new SQLException("Invalid number of rows: multiple");

    rs.close();

    return value;
  }

  /** Retrieves the last inserted auto-increment id created within the current transaction. */
  public static int getLastInsertedID (Connection connection) throws SQLException {
    Statement statement = null;
    try {
      statement = connection.createStatement();

      int id = getLastInsertedID(statement);

      statement.close();
      statement = null;

      return id;
    }
    finally {
      QueryUtil.closeNoThrow(statement);
    }
  }


  /** Retrieves the last inserted auto-increment id created within the current transaction. */
  public static int getLastInsertedID (Statement statement) throws SQLException {
    return getSingleIntValueResult(statement, "SELECT LAST_INSERT_ID()");
  }

  /** Retrieves the last inserted auto-increment id created within the current transaction. */
  public static long getLastInsertedLongID (Statement statement) throws SQLException {
    return getSingleLongValueResult(statement, "SELECT LAST_INSERT_ID()");
  }

  /** Retrieves the last inserted auto-increment id created within the current transaction. */
  public static int getCheckLastInsertedLongIDAsInt (Statement statement) throws SQLException {
    long longValue = getLastInsertedLongID(statement);

    int intValue = (int) longValue;
    if (intValue != longValue)
      throw new SQLException("Found long ID that cannot fit within int constraints");

    return intValue;
  }

  /** Retrieves the current date-time according to the database. */
  public static Date getCurrentDatabaseDateTime (Connection connection) throws SQLException {
    Statement statement = connection.createStatement();
    Date date = getCurrentDatabaseDateTime(statement);
    statement.close();
    return date;
  }

  /** Retrieves the current date-time according to the database. */
  public static Date getCurrentDatabaseDateTime (Statement statement) throws SQLException {
    ResultSet resultSet = statement.executeQuery(
      "SELECT NOW()"
    );
    resultSet.next();
    Date date = resultSet.getTimestamp(1);
    resultSet.close();
    return date;
  }

  public static int getCheckLongIDAsInt (ResultSet rs, String fieldName) throws SQLException {
    long longValue = rs.getLong(fieldName);

    int intValue = (int) longValue;
    if (intValue != longValue)
      throw new SQLException("Found long ID that cannot fit within int constraints");

    return intValue;
  }

  /** Retrieves a possibly null integer values. */
  public static Integer getInteger (ResultSet rs, String fieldName) throws SQLException {
    int value = rs.getInt(fieldName);
    return rs.wasNull() ? null : new Integer(value);
  }

  /** Retrieves a possibly null integer values. */
  public static Integer getInteger (ResultSet rs, int fieldIndex) throws SQLException {
    int value = rs.getInt(fieldIndex);
    return rs.wasNull() ? null : new Integer(value);
  }

  /** Retrieves a possibly null short values. */
  public static Short getShort (ResultSet rs, String fieldName) throws SQLException {
    short value = rs.getShort(fieldName);
    return rs.wasNull() ? null : new Short(value);
  }

  /** Retrieves a possibly null short values. */
  public static Short getShort (ResultSet rs, int fieldIndex) throws SQLException {
    short value = rs.getShort(fieldIndex);
    return rs.wasNull() ? null : new Short(value);
  }

  /** Retrieves a possibly null long values. */
  public static Long getLong (ResultSet rs, String fieldName) throws SQLException {
    long value = rs.getLong(fieldName);
    return rs.wasNull() ? null : new Long(value);
  }

  /** Retrieves a possibly null long values. */
  public static Long getLong (ResultSet rs, int fieldIndex) throws SQLException {
    long value = rs.getLong(fieldIndex);
    return rs.wasNull() ? null : new Long(value);
  }

  /** Retrieves a BigDecimal value from a fixed (scale = 2) int field. */
  public static BigDecimal getFixedIntBigDecimal (ResultSet rs, String fieldName) throws SQLException {
    int value = rs.getInt(fieldName);
    return BigDecimal.valueOf(value, 2);
  }

  /** Retrieves a BigDecimal value from a fixed (scale = 2) int field. */
  public static BigDecimal getFixedIntegerBigDecimal (ResultSet rs, String fieldName) throws SQLException {
    Integer value = QueryUtil.getInteger(rs, fieldName);
    if(value != null) {
      return BigDecimal.valueOf(value, 2);
    }
    else {
      return null;
    }
  }

  /** Retrieves a possibly null double values. */
  public static Double getDouble (ResultSet rs, String fieldName) throws SQLException {
    double value = rs.getDouble(fieldName);
    return rs.wasNull() ? null : new Double(value);
  }

  /** Retrieves a possibly null boolean interpreted integer value. */
  public static Boolean getPossiblyNullIntBoolean (ResultSet rs, String fieldName) throws SQLException {
    int value = rs.getInt(fieldName);
    return (rs.wasNull() ? null : value != 0);
  }

  /** Retrieves a possibly null boolean interpreted integer value. */
  public static Boolean getPossiblyNullIntBoolean (ResultSet rs, int fieldIndex) throws SQLException {
    int value = rs.getInt(fieldIndex);
    return (rs.wasNull() ? null : value != 0);
  }
  
  /** Retrieves a boolean interpreted integer value. */
  public static boolean getIntBoolean (ResultSet rs, String fieldName) throws SQLException {
    return (rs.getInt(fieldName) != 0);
  }

  /** Retrieves a boolean interpreted integer value. */
  public static boolean getIntBoolean (ResultSet rs, int fieldIndex) throws SQLException {
    return (rs.getInt(fieldIndex) != 0);
  }

  /** Retrieves a possibly null boolean interpreted integer value. */
  public static Boolean getIntBooleanObj (ResultSet rs, String fieldName) throws SQLException {
    boolean value = (rs.getInt(fieldName) != 0);
    return rs.wasNull() ? null : Boolean.valueOf(value);
  }

  /** Retrieves a possibly null enum calue. */
  public static <Type extends Enum<Type>> Type getEnum (ResultSet rs, String fieldName, Class<Type> enumClass, boolean upperCaseStr) throws SQLException {
    String value = rs.getString(fieldName);

    if(upperCaseStr && value != null) {
      value = value.toUpperCase();
    }

    try {
      return (Type) ((value != null)
                       ? Enum.valueOf(enumClass, value)
                       : null);
    }
    catch (IllegalArgumentException iae) {
      SQLException sqle = new SQLException("Invalid value (\"" + value + "\") for enum " + enumClass.getName());
      sqle.initCause(iae);
      throw sqle;
    }
  }

  public static <EnumType extends VEnum> EnumType getVEnum (ResultSet resultSet, String fieldName, VEnumSet vEnumSet) throws SQLException {
    Integer id = QueryUtil.getInteger(resultSet, fieldName);
    return (EnumType) ((id != null) ? vEnumSet.get(id) : null);
  }

  /** Performs the update, then checks to make sure the specified number of rows were affected. */
  public static void updateCheckRowCount (Statement statement, String sql, int expectedRowCow) throws SQLException {
    executeUpdateCheckRowCount(statement, sql, expectedRowCow);
  }

  /** Performs the update, then checks to make sure the specified number of rows were affected. */
  public static void executeUpdateCheckRowCount (Statement statement, String sql, int expectedRowCow) throws SQLException {
    int rowCount = statement.executeUpdate(sql);
    if (rowCount != expectedRowCow)
      throw new SQLException("Update row count does not match expected row count (" + rowCount + " != " + expectedRowCow + ")");
  }

  /**
   * Executes the SQL, checks that one-and-only-one row was inserted, then
   * retrieves and returns the last inserted ID.
   */
  public static int executeInsertGetLastInsertedIntID (Statement statement, String sql) throws SQLException {
    updateCheckRowCount(statement, sql, 1);
    return getLastInsertedID(statement);
  }

  /**
   * Delete a set of rows in chunks - committing each chunk.<br>
   * <br>
   * NOTE: If the call fails the data may be left partially deleted.
   * 
   * @param connection The connection to use / commit.
   * @param deleteSQL The sql to use to perform the deletes - note: must allow a limit to be attached.
   * @param chunkSize The number of rows to delete at a time.
   */
  public static void deleteChunkCommitLoop (Connection connection, String deleteSQL, int chunkSize) throws SQLException {
    while (true) {
      
      // Delete a chunk of rows and commit

      Statement statement = connection.createStatement();

      int numRowsAffected = statement.executeUpdate(
        deleteSQL + " LIMIT " + chunkSize
      );

      statement.close();
      connection.commit();

      // Exit if we deleted the last set of rows

      if (numRowsAffected < chunkSize)
        break;
    }
  }

  public static void insertSelectChunkCommitLoop (Connection connection, String insertPortion, String selectPortion, String fromAndWherePortion, int chunkSize) throws SQLException {
    insertSelectChunkCommitLoop(connection, insertPortion, selectPortion, fromAndWherePortion, chunkSize, true);
  }

  public static void insertSelectChunkCommitLoop (Connection connection, String insertPortion, String selectPortion, String fromAndWherePortion, int chunkSize, boolean commit) throws SQLException {

    Statement statement = connection.createStatement();
    int totalRows = getSingleIntValueResult(statement, "SELECT COUNT(*) " + fromAndWherePortion);
    statement.close();

    int startRow = 0;
    while (startRow < totalRows) {
      int updateCount = Math.min(totalRows - startRow, chunkSize);

      // Insert a chunk of rows and commit

      statement = connection.createStatement();

      updateCheckRowCount(
        statement,
        insertPortion
          + " " + selectPortion
          + " " + fromAndWherePortion
          + " LIMIT " + startRow + ", " + updateCount,
        updateCount
      );

      statement.close();
      
      if(commit) {
        connection.commit();
      }

      startRow += updateCount;
    }
  }

  public static void keyedInsertSelectChunkCommitLoop (
    Connection connection,
    String keySelectKeyField, String keySelectTableList, String keySelectJoins, String keySelectWhereCriteria,
    String insertPortion, String selectPortion, String selectTables, String selectJoins, String whereCriteria, String selectKeyField, int chunkSize, String tempTableName) throws SQLException
  {
    // Setup

    Statement statement = connection.createStatement();

    statement.executeUpdate(
      "DROP TABLE IF EXISTS " + tempTableName
    );

    statement.executeUpdate(
      "SET @KeyRowIndex = -1"
    );

    statement.executeUpdate(
      "CREATE TABLE " + tempTableName + " AS"
      + " SELECT @KeyRowIndex := @KeyRowIndex + 1 tempInsertSelectKeyIndex_index, " + keySelectKeyField + " tempInsertSelectKeyIndex_key"
      + " FROM (" + keySelectTableList + ")"
      + ((keySelectJoins != null) ? " " + keySelectJoins : "")
      + ((keySelectWhereCriteria != null) ? " WHERE " + keySelectWhereCriteria : "")
    );

    int totalRows = QueryUtil.getSingleIntValueResult(statement, "SELECT @KeyRowIndex") + 1;

    statement.executeUpdate(
      "CREATE INDEX orderIndex ON " + tempTableName + " (tempInsertSelectKeyIndex_index)"
    );

    statement.close();
    connection.commit();

    // Perform insert-select chunks

    int startKeyRow = 0;
    while (startKeyRow < totalRows) {
      int keyRowCount = Math.min(totalRows - startKeyRow, chunkSize);
      int limitKeyRow = startKeyRow + keyRowCount;

      executeUpdate(
        connection,
        insertPortion
          + " " + selectPortion
          + " FROM (" + tempTableName + ", " + selectTables + ")"
          + ((selectJoins != null) ? " " + selectJoins : "")
          + " WHERE (" + startKeyRow + " <= tempInsertSelectKeyIndex_index AND tempInsertSelectKeyIndex_index < " + limitKeyRow + ")"
          + " AND " + selectKeyField + " = tempInsertSelectKeyIndex_key"
          + " AND (" + whereCriteria + ")"
      );

      connection.commit();

      startKeyRow += keyRowCount;
    }

    // Cleanup

    executeUpdate(
      connection,
      "DROP TABLE " + tempTableName
    );

    connection.commit();
  }

  public static void deleteChunkCommitLoop (Connection connection, String deleteTableList, String usingTableList, String whereCriteria, String keyField, int chunkSize, String tempTableName) throws SQLException {
    performUpdateChunkCommitLoop(
      connection, usingTableList, whereCriteria, keyField, chunkSize, tempTableName,
      "DELETE FROM " + deleteTableList
      + " USING " + tempTableName + ", " + usingTableList
    );
  }

  public static int updateChunkCommitLoop (Connection connection, String tableList, String whereCriteria, String keyField, String updateAssignments, int chunkSize, String tempTableName) throws SQLException {
    return performUpdateChunkCommitLoop(
      connection, tableList, whereCriteria, keyField, chunkSize, tempTableName,
      "UPDATE " + tempTableName + ", " + tableList
      + " SET " + updateAssignments
    );
  }

  private static int performUpdateChunkCommitLoop (Connection connection, String tableList, String whereCriteria, String keyField, int chunkSize, String tempTableName, String updatePrefixSQL) throws SQLException {

    // Setup an index table to drive the delete

    Statement statement = connection.createStatement();

    statement.executeUpdate(
      "DROP TABLE IF EXISTS " + tempTableName
    );

    statement.executeUpdate(
      "SET @RowIndex = -1"
    );

    statement.executeUpdate(
      "CREATE TABLE " + tempTableName + " AS"
      + " SELECT @RowIndex := @RowIndex + 1 tempUpdateIndex_index, " + keyField + " tempUpdateIndex_key"
      + " FROM " + tableList
      + " WHERE " + whereCriteria
    );

    int totalRows = QueryUtil.getSingleIntValueResult(statement, "SELECT @RowIndex") + 1;

    statement.executeUpdate(
      "CREATE INDEX row_id ON " + tempTableName + " (tempUpdateIndex_index)"
    );

    statement.close();
    connection.commit();

    // Update chunks of rows, committing each set, until all the rows are updated

    int startRow = 0;
    while (startRow < totalRows) {
      int updateCount = Math.min(totalRows - startRow, chunkSize);
      int limitRow = startRow + updateCount;

      statement = connection.createStatement();

      updateCheckRowCount(
        statement,
        updatePrefixSQL
        + " WHERE (" + startRow + " <= tempUpdateIndex_index AND tempUpdateIndex_index < " + limitRow + ")"
        + " AND " + keyField + " = tempUpdateIndex_key"
        + " AND (" + whereCriteria + ")",
        updateCount
      );

      statement.close();
      connection.commit();

      startRow += updateCount;
    }

    // Cleanup

    statement = connection.createStatement();

    statement.executeUpdate(
      "DROP TABLE " + tempTableName
    );

    statement.close();
    connection.commit();

    return totalRows;
  }

  /** Checks if the specified exception represents a duplicate key error. */
  // NOTE: THIS METHOD MUST BE KEPT IN SYNC WITH THE JDBC DRIVER VERSION!
  public static boolean isDuplicateKeyException (SQLException sqle) {
    String message = sqle.getMessage();
    return message.indexOf("Duplicate entry") >= 0
        && message.indexOf("for key") >= 0;
  }

  /** Checks if the specified exception represents a table exists error. */
  // NOTE: THIS METHOD MUST BE KEPT IN SYNC WITH THE JDBC DRIVER VERSION!
  public static boolean isTableExistsException (SQLException sqle) {
    String message = sqle.getMessage();
    return message.indexOf("Table ") >= 0
           && message.indexOf(" already exists") >= 0;
  }

  /** Checks if the specified exception represents a lock-wait timeout. */
  // NOTE: THIS METHOD MUST BE KEPT IN SYNC WITH THE JDBC DRIVER VERSION!
  public static boolean isLockWaitTimeoutException (SQLException sqle) {
    return sqle.getErrorCode() == 1205
        && sqle.getMessage().indexOf("Lock wait timeout") >= 0;
  }

  public static List<String> buildListFromString (String value) {
    return StringUtil.buildListFromString(value, StringUtil.MYSQL_SEPERATOR_SEQUENCE_ESCAPED);
  }
  
  public static String buildStringFromList(List<String> list) {
    return StringUtil.buildStringFromList(list, StringUtil.MYSQL_STORAGE_SEPERATOR_SEQUENCE);
  }
  
  public static List<Integer> getIntegerList(Statement statement, String sql, String columnName) throws SQLException {
    
    List<Integer> list = new ArrayList<Integer>();
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      list.add(resultSet.getInt(columnName));
    }
    
    resultSet.close();
    
    return list;
  }
  
  public static <T extends DBObject> List<T> retrieveDataObjectList(Statement statement, String tableName, String criteria, Class<T> dbObjectClass) throws SQLException {
    
    return retrieveDataObjectList(statement, tableName, criteria, null, dbObjectClass);
  }
  
  public static <T extends DBObject> List<T> retrieveDataObjectList(Statement statement, String tableName, String criteria, String orderBy, Class<T> dbObjectClass) throws SQLException {
    
    String sql = " SELECT *"
               + " FROM `" + tableName.replace("`", "") + "`"
               + " WHERE " + (criteria == null ? "1" : criteria)
               + (orderBy == null ? "" : (" ORDER BY " + orderBy));
    
    ResultSet resultSet = statement.executeQuery(sql);
    List<T> dbObjectList = new ArrayList<T>();
    
    while(resultSet.next()) {
      
      dbObjectList.add(retrieveDataObject(resultSet, dbObjectClass));
    }
    
    resultSet.close();
    
    return dbObjectList;
  }
  
  public static <KeyType, PassingType extends DBObject> Map<KeyType, PassingType> retrieveDataObjectMap(Statement statement, String tableName, String criteria, Class<PassingType> passingTypeClass, GenericAccessor<PassingType, KeyType> genericAccessor) throws SQLException {
    
    String sql = " SELECT *"
        + " FROM `" + tableName.replace("`", "") + "`"
        + " WHERE " + (criteria == null ? "1" : criteria);

    ResultSet resultSet = statement.executeQuery(sql);
    Map<KeyType, PassingType> map = new HashMap<KeyType, PassingType>(); 
    
    while(resultSet.next()) {
    
      PassingType passingType = retrieveDataObject(resultSet, passingTypeClass);
      map.put(genericAccessor.get(passingType), passingType);
    }
    
    resultSet.close();
  
    return map;
  }

  public static <T extends DBObject> T retrieveDataObject(Statement statement, String tableName, String criteria, Class<T> dbObjectClass) throws SQLException {
    
    String sql = " SELECT *"
               + " FROM `" + tableName.replace("`", "") + "`"
               + " WHERE " + (criteria == null ? "1" : criteria);
    
    return retrieveDataObject(statement, sql, dbObjectClass);
  }
  
  public static <T extends DBObject> T retrieveDataObject(Statement statement, String sql, Class<T> dbObjectClass) throws SQLException {
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return retrieveDataObject(resultSet, dbObjectClass);
    }
    
    resultSet.close();
    
    return null;
  }
  
  public static <T extends DBObject> T retrieveDataObject(ResultSet resultSet, Class<T> dbObjectClass) throws SQLException {
    
    try {
      T dbObject = dbObjectClass.newInstance();
    
      dbObject.retrieveFromResultSet(resultSet);

      return dbObject;
    }
    catch(Exception exception) {
      
      if(exception instanceof SQLException) {
        
        throw (SQLException)exception;
      }
      
      throw new SQLException(exception);
    }
  }
}