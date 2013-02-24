package org.kinslayermud.web.admin;

import org.kinslayermud.userlog.UserLog;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;

public class LoadUserLogAction extends StandardAction {

  private static String SUCCESS_FORWARD = "Success";

  protected static String USER_LOG_ID_PARAMETER = "UserLogID";
  
  public String execute(WebSupport webSupport) throws Exception {
    
    int userLogId = Integer.valueOf(request.getParameter(USER_LOG_ID_PARAMETER));
    
    UserLog userLog = webSupport.getUserLog(userLogId);
    
    request.setAttribute("UserLog", userLog);
    return SUCCESS_FORWARD;
  }
}
