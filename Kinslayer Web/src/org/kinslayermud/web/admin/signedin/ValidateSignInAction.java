package org.kinslayermud.web.admin.signedin;

import org.kinslayermud.character.User;
import org.kinslayermud.util.WebSupport;

public abstract class ValidateSignInAction extends org.kinslayermud.web.playerportal.signedin.ValidateSignInAction {

  protected final String FAILURE_FORWARD = "Failure";
  
  public abstract String executeAdmin(WebSupport webSupport, User user) throws Exception;
  
  public String execute(WebSupport webSupport, User user) throws Exception {
    
    if(user.getLevel() < 100) {
      
      return FAILURE_FORWARD;
    }
    
    return executeAdmin(webSupport, user);
  }
}
