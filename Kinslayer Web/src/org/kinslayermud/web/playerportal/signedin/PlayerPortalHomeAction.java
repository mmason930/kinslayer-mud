package org.kinslayermud.web.playerportal.signedin;

import java.util.List;

import org.kinslayermud.character.User;
import org.kinslayermud.userlog.UserLogRecord;
import org.kinslayermud.util.WebSupport;

public class PlayerPortalHomeAction extends ValidateSignInAction {

  protected String SUCCESS_FORWARD = "Success";
  
  public String execute(WebSupport webSupport, User user) throws Exception {
    
    List<UserLogRecord> userLogRecords = webSupport.getUserLogRecords(user.getUserId());
    
    request.setAttribute("UserLogRecords", userLogRecords);
    request.setAttribute("User", user);
    
    return SUCCESS_FORWARD;
  }
}
