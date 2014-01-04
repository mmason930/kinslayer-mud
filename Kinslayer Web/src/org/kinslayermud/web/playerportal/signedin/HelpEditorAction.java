package org.kinslayermud.web.playerportal.signedin;

import org.kinslayermud.character.User;
import org.kinslayermud.util.WebSupport;

public class HelpEditorAction extends ValidateSignInAction {

  protected String SUCCESS_FORWARD = "Success";
  protected String FAILURE_FORWARD = "Failure";
  
  public String execute(WebSupport webSupport, User user) throws Exception {
    
    request.setAttribute("User", user);
    
    if(user.getLevel() < 100) {
      
      return FAILURE_FORWARD;
    }
    
    return SUCCESS_FORWARD;
  }
}
