package org.kinslayermud.web.playerportal.signedin;

import java.util.List;

import org.kinslayermud.character.User;
import org.kinslayermud.userlog.UserLog;
import org.kinslayermud.userlog.UserLogRecord;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.WebSupport;

public class ViewUserLogAction extends ValidateSignInAction {

  protected String SUCCESS_FORWARD = "Success";
  protected String FAILURE_FORWARD = "Failure";
  
  protected String USERLOGID_PARAMETER = "UserLogId";
  
  public String execute(WebSupport webSupport, User user) throws Exception {
    
    String userLogIdString = request.getParameter(USERLOGID_PARAMETER);
    
    if(userLogIdString != null && MiscUtil.isValidIntString(userLogIdString)) {
      
      UserLog userLog = webSupport.getUserLog(Integer.valueOf(userLogIdString));
      
      if(userLog.getUserId() == user.getUserId()) {
        
        request.setAttribute("UserLog", userLog);
        return SUCCESS_FORWARD;
      }
    }
    
    return FAILURE_FORWARD;
  }
}
