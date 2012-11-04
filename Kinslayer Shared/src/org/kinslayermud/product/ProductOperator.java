package org.kinslayermud.product;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

public interface ProductOperator {

  public Product getProduct(ResultSet resultSet) throws SQLException;
  public void putProduct(Statement statement, Product product) throws SQLException;
}
