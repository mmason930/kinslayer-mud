package org.kinslayermud.web.admin.signedin;

import org.kinslayermud.character.User;
import org.kinslayermud.util.WebSupport;

public class OListAction extends ValidateSignInAction {

  public String executeAdmin(WebSupport webSupport, User user) throws Exception {
    
    if(user.getLevel() < 103) {
      
      return FAILURE_FORWARD;
    }
    
    return SUCCESS_FORWARD;
  }
}
