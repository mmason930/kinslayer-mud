package org.kinslayermud.dbutils;

import java.sql.Connection;
import java.sql.Statement;

public interface StatementConnectionExecutor <T> {

  public T execute(Connection connection, Statement statement) throws Exception;
}
