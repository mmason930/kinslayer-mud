package org.kinslayermud.web.playerportal.signedin;

import javax.servlet.http.Cookie;

import org.kinslayermud.character.User;
import org.kinslayermud.util.WebSupport;

public class SignOutAction extends ValidateSignInAction {

  protected final String SUCCESS_FORWARD = "Success";
  public String execute(WebSupport webSupport, User user) throws Exception {

    String sessionId = (String)request.getAttribute("SessionId");
    webSupport.performPlayerPortalSignOut(sessionId, user.getUserId());
    
    Cookie cookie = new Cookie("SESSIONID", null);
    cookie.setMaxAge(0);
    cookie.setPath("/");
    response.addCookie(cookie);
    
    cookie = new Cookie("SESSIONUSERID", null);
    cookie.setMaxAge(0);
    cookie.setPath("/");
    response.addCookie(cookie);
    
    cookie = new Cookie("SESSIONUSERNAME", null);
    cookie.setMaxAge(0);
    cookie.setPath("/");
    response.addCookie(cookie);

    return SUCCESS_FORWARD;
  }

}
