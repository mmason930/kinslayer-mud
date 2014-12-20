package org.kinslayermud.web.playerportal.signedin;

import org.kinslayermud.character.User;
import org.kinslayermud.util.WebSupport;

public class ViewMapAction extends ValidateSignInAction {

  protected final String SUCCESS_FORWARD = "Success";
  protected final String FAILURE_FORWARD = "Failure";
  
  public String execute(WebSupport webSupport, User user) throws Exception {
    
    request.setAttribute("User", user);
    
    return SUCCESS_FORWARD;
  }
}
