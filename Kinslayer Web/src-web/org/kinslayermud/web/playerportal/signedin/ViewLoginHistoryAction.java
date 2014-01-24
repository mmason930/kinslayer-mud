package org.kinslayermud.web.playerportal.signedin;

import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import org.kinslayermud.character.User;
import org.kinslayermud.user.login.UserLogin;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.WebSupport;

public class ViewLoginHistoryAction extends ValidateSignInAction {

  protected String SUCCESS_FORWARD = "Success";
  protected String FAILURE_FORWARD = "Failure";
  
  protected int NUMBER_OF_COLUMNS = 2;
  
  public String execute(WebSupport webSupport, User user) throws Exception {
    
    List<UserLogin> userLogins = webSupport.getUserLoginsByUserId(user.getUserId());
    
    Collections.sort(userLogins, new Comparator<UserLogin>() {

      public int compare(UserLogin arg0, UserLogin arg1) {
        
        return arg0.getLoginDatetime().compareTo(arg1.getLoginDatetime());
      }
    });
    
    request.setAttribute("UserLoginTable", MiscUtil.createOrderedColumnTable(userLogins, NUMBER_OF_COLUMNS));
    request.setAttribute("UserLogins", userLogins);
    request.setAttribute("User", user);
    
    return SUCCESS_FORWARD;
  }
}
