package org.kinslayermud.dbutils;

import java.sql.Statement;

public interface StatementExecutor <T> {

  T execute(Statement statement) throws Exception;
}
