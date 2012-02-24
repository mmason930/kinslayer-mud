package mud.scripts.fundamentals;

import java.sql.Connection;

public interface ExternalScriptProcess {

  public void run(Connection connection);

}
