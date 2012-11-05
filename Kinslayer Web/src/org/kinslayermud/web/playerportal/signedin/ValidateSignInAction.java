package org.kinslayermud.web.playerportal.signedin;

import javax.servlet.http.Cookie;

import org.kinslayermud.character.User;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;
import org.kinslayermud.web.util.HttpUtil;

public abstract class ValidateSignInAction extends StandardAction {

  protected final String FAILURE_FORWARD = "Failure";
  
  public abstract String execute(WebSupport webSupport, User user) throws Exception;
  
  public String execute(WebSupport webSupport) throws Exception {
    
    Cookie cookie = HttpUtil.getCookieByName(request, "SESSIONID");
    
    if(cookie == null) {
      
      return FAILURE_FORWARD;
    }
    
    User user = webSupport.getUserFromSession(cookie.getValue());
    
    if(user == null) {
      
      return FAILURE_FORWARD;
    }
    
    return execute(webSupport, user);
  }
}
