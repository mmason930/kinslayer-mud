package org.kinslayermud.web.playerportal.signedin;

import java.util.List;

import org.kinslayermud.character.User;
import org.kinslayermud.userlog.UserLogRecord;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.WebSupport;

public class ViewUserLogsAction extends ValidateSignInAction {

  protected final String SUCCESS_FORWARD = "Success";
  protected final String FAILURE_FORWARD = "Failure";
  
  protected final int NUMBER_OF_COLUMNS = 3;
  
  public String execute(WebSupport webSupport, User user) throws Exception {
    
    List<UserLogRecord> userLogRecords = webSupport.getUserLogRecords(user.getUserId());
    
    request.setAttribute("UserLogRecordsTable", MiscUtil.createOrderedColumnTable(userLogRecords, NUMBER_OF_COLUMNS));
    request.setAttribute("UserLogRecords", userLogRecords);
    return SUCCESS_FORWARD;
  }
}
