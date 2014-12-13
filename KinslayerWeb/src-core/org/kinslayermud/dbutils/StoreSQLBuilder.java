package org.kinslayermud.dbutils;

import java.math.BigDecimal;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import org.kinslayermud.dual.Dual;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.util.SQLUtil;

public class StoreSQLBuilder {
  
  List<Dual<String, String>> columns;
  String tableName;
  
  public StoreSQLBuilder(String tableName) {
    
    this.columns = new ArrayList<Dual<String, String>>();
    this.tableName = tableName;
  }
  
  public StoreSQLBuilder putEscapedString(String columnName, String escapedString) {
    
    columns.add(new Dual<String, String>(columnName, escapedString));
    return this;
  }
  
  public StoreSQLBuilder put(String columnName, String value) {
    
    return putEscapedString(columnName, SQLUtil.escapeQuoteString(value));
  }
  
  public StoreSQLBuilder put(String columnName, Integer value) {
    
    return putEscapedString(columnName, value == null ? "NULL" : String.valueOf(value));
  }
  
  public StoreSQLBuilder put(String columnName, int value) {
    
    return putEscapedString(columnName, String.valueOf(value));
  }
  
  public StoreSQLBuilder put(String columnName, boolean value) {
    
    return putEscapedString(columnName, SQLUtil.encodeBooleanInt(value));
  }

  public StoreSQLBuilder put(String columnName, Boolean value) {
    
    return putEscapedString(columnName, SQLUtil.encodeBooleanInt(value));
  }
  
  public StoreSQLBuilder put(String columnName, Long value) {
    
    return putEscapedString(columnName, value == null ? "NULL" : String.valueOf(value));
  }

  public StoreSQLBuilder put(String columnName, long value) {
    
    return putEscapedString(columnName, String.valueOf(value));
  }
  
  public StoreSQLBuilder put(String columnName, BigDecimal value) {
    
    return putEscapedString(columnName, String.valueOf(value));
  }
  
  public StoreSQLBuilder putMoney(String columnName, BigDecimal moneyValue) {
    
    return putEscapedString(columnName, String.valueOf(moneyValue.movePointRight(2).intValue()));
  }
  
  public StoreSQLBuilder put(String columnName, java.sql.Date value) throws SQLException {
    return putEscapedString(columnName, (value != null) ? SQLUtil.encodeQuoteTimestamp(value) : "NULL");
  }
  
  public StoreSQLBuilder put(String columnName, Date value) throws SQLException {
    return putEscapedString(columnName, (value != null) ? SQLUtil.encodeQuoteDate(value) : "NULL");
  }
  
  public StoreSQLBuilder put(String columnName, VEnum vEnum) throws SQLException {
    return putEscapedString(columnName, vEnum == null ? null : SQLUtil.escapeQuoteString(vEnum.toString()));
  }
  
  public String generateInsert() {
    
    StringBuilder stringBuilder = new StringBuilder();
    
    StringBuilder columnNameStringBuilder = new StringBuilder();
    StringBuilder columnValueStringBuilder = new StringBuilder();
    stringBuilder.append("INSERT INTO `").append(tableName.replace("`", "")).append("`(");
    
    boolean isFirst = true;
    for(Dual<String, String> columnDual : columns) {
      
      columnNameStringBuilder.append(isFirst ? "" : ",").append("`").append(columnDual.getObject1()).append("`");
      columnValueStringBuilder.append(isFirst ? "" : ",").append(columnDual.getObject2());
      isFirst = false;
    }
    
    stringBuilder.append(columnNameStringBuilder.toString())
                 .append(")VALUES(")
                 .append(columnValueStringBuilder.toString())
                 .append(")");
    
    return stringBuilder.toString();
  }
  
  public String generateUpdate(short id) {
    
    return generateUpdate("id", id);
  }
  
  public String generateUpdate(int id) {
    
    return generateUpdate("id", id);
  }
  
  public String generateUpdate(long id) {
    
    return generateUpdate("id", id);
  }
  
  public String generateUpdate(String idColumnName, short id) {
    
    return generateUpdate("`" + idColumnName.replace("`", "") + "`" + "=" + id);
  }
  
  public String generateUpdate(String idColumnName, int id) {
    
    return generateUpdate("`" + idColumnName.replace("`", "") + "`" + "=" + id);
  }
  
  public String generateUpdate(String idColumnName, long id) {
    
    return generateUpdate("`" + idColumnName.replace("`", "") + "`" + "=" + id);
  }
  
  public String generateUpdate(String whereClause) {
    
    StringBuilder stringBuilder = new StringBuilder();
    
    stringBuilder.append("UPDATE `" + tableName.replace("`", "") + "` SET ");
    
    boolean isFirst = true;
    for(Dual<String, String> columnDual : columns) {
      
      stringBuilder.append(isFirst ? "" : ",").append("`").append(columnDual.getObject1()).append("`=").append(columnDual.getObject2());
      isFirst = false;
    }
    
    stringBuilder.append(" WHERE " + whereClause);
    
    return stringBuilder.toString();
  }
}