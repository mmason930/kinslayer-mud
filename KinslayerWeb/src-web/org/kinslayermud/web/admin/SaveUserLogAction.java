package org.kinslayermud.web.admin;

import org.kinslayermud.userlog.UserLog;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;

public class SaveUserLogAction extends StandardAction {

  private static String SUCCESS_FORWARD = "Success";

  protected static String USER_LOG_ID_PARAMETER = "UserLogID";
  protected static String CONSOLE_OUTPUT_PARAMETER = "ConsoleOutput";
  
  public String execute(WebSupport webSupport) throws Exception {
    
    int userLogId = Integer.valueOf(request.getParameter(USER_LOG_ID_PARAMETER));
    String consoleOutput = request.getParameter(CONSOLE_OUTPUT_PARAMETER);
    
    UserLog userLog = webSupport.getUserLog(userLogId);
    userLog.setConsoleOutput(consoleOutput);
    
    webSupport.putUserLog(userLog);
    
    request.setAttribute("UserLog", userLog);
    return SUCCESS_FORWARD;
  }
}
