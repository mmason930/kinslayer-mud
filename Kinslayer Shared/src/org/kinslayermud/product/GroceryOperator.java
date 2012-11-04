package org.kinslayermud.product;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import org.kinslayermud.util.QueryUtil;
import org.kinslayermud.util.SQLUtil;


public class GroceryOperator extends BaseProductOperator {

  public Product getProduct(ResultSet resultSet) throws SQLException {

    Grocery grocery = new Grocery();
    
    fillInBaseProperties(resultSet, grocery);
    
    grocery.setBrand(resultSet.getString("brand"));
    grocery.setNumberOfItems(QueryUtil.getInteger(resultSet, "number_of_items"));
    
    return grocery;
  }

  public void putProduct(Statement statement, Product product) throws SQLException {
    
    Grocery grocery = (Grocery)product;
    boolean isNew = grocery.isNew();
    String sql;
    putBaseProduct(statement, grocery);
    
    if(isNew) {
      
      sql = " INSERT INTO grocery("
          + "   `product_id`,"
          + "   `brand`,"
          + "   `number_of_items`"
          + " ) VALUES( "
          + grocery.getId() + ","
          + SQLUtil.escapeQuoteString(grocery.getBrand()) + ","
          + grocery.getNumberOfItems() + ")";
      
      statement.executeUpdate(sql);
    }
    else {
      
      sql = " UPDATE grocery SET"
          + "   brand = " + SQLUtil.escapeQuoteString(grocery.getBrand()) + ","
          + "   number_of_items = " + grocery.getNumberOfItems()
          + " WHERE product_id = " + grocery.getId();
      
      statement.executeQuery(sql);
    }
  }
}
