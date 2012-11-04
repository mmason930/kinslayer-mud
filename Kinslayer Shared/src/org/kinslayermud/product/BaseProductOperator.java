package org.kinslayermud.product;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import org.kinslayermud.util.QueryUtil;
import org.kinslayermud.util.SQLUtil;


public abstract class BaseProductOperator implements ProductOperator {

  protected static String JOIN_COLUMNS = " LEFT JOIN book ON book.product_id = product.id";
  
  protected void fillInBaseProperties(ResultSet resultSet, Product product) throws SQLException {
    
    product.setId(resultSet.getInt("product.id"));
    product.setName(resultSet.getString("product.name"));
    product.setListPrice(QueryUtil.getInteger(resultSet, "product.list_price"));
    product.setManufacturer(resultSet.getString("product.manufacturer"));
    product.setAsin(resultSet.getString("product.asin"));
    product.setWeight(QueryUtil.getInteger(resultSet, "product.weight"));
    product.setDimensions(resultSet.getString("product.dimensions"));
    product.setProductCode(resultSet.getString("product.product_code"));
    product.setCreatedDatetime(resultSet.getTimestamp("product.created_datetime"));
    product.setLastModifiedDatetime(resultSet.getTimestamp("product.last_modified_datetime"));
  }

  protected void putBaseProduct(Statement statement, Product product) throws SQLException {
    
    String sql;
    if(product.isNew()) {
      
      sql = " INSERT INTO product("
          + "   `product_code`,"
          + "   `name`,"
          + "   `list_price`,"
          + "   `asin`,"
          + "   `manufacturer`,"
          + "   `weight`,"
          + "   `dimensions`,"
          + "   `type`,"
          + "   `created_datetime`,"
          + "   `last_modified_datetime`"
          + ") VALUES ("
          + SQLUtil.escapeQuoteString(product.getProductCode()) + ","
          + SQLUtil.escapeQuoteString(product.getName()) + ","
          + product.getListPrice() + ","
          + SQLUtil.escapeQuoteString(product.getAsin()) + ","
          + SQLUtil.escapeQuoteString(product.getManufacturer()) + ","
          + product.getWeight() + ","
          + SQLUtil.escapeQuoteString(product.getDimensions()) + ","
          + product.getType() + ","
          + SQLUtil.encodeQuoteDate(product.getCreatedDatetime()) + ","
          + SQLUtil.encodeQuoteDate(product.getLastModifiedDatetime()) + ")";
      
      statement.executeUpdate(sql);
      
      product.setId(QueryUtil.getLastInsertedID(statement));
    }
    else {
      
      sql = " UPDATE product SET"
          + "   product_code = " + SQLUtil.escapeQuoteString(product.getProductCode()) + ","
          + "   name = " + SQLUtil.escapeQuoteString(product.getName()) + ","
          + "   list_price = " + product.getListPrice() + ","
          + "   asin = " + SQLUtil.escapeQuoteString(product.getAsin()) + ","
          + "   manufacturer = " + SQLUtil.escapeQuoteString(product.getManufacturer()) + ","
          + "   weight = " + product.getWeight() + ","
          + "   dimensions = " + SQLUtil.escapeQuoteString(product.getDimensions()) + ","
          + "   type = " + product.getType() + ","
          + "   created_datetime = " + SQLUtil.encodeQuoteDate(product.getCreatedDatetime()) + ","
          + "   last_modified_datetime = " + SQLUtil.encodeQuoteDate(product.getLastModifiedDatetime())
          + " WHERE id = " + product.getId();
      
      statement.executeUpdate(sql);
    }
  }
  
}
