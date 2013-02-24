package org.kinslayermud.web.playerportal.signedin;

import org.kinslayermud.character.User;
import org.kinslayermud.util.WebSupport;

public class PlayerPortalHomeAction extends ValidateSignInAction {

  protected String SUCCESS_FORWARD = "Success";
  
  public String execute(WebSupport webSupport, User user) throws Exception {
    
    
    request.setAttribute("User", user);
    
    return SUCCESS_FORWARD;
  }
}
