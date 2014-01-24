package org.kinslayermud.web.playerportal;

import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.util.LinkedList;
import java.util.List;

import javax.servlet.http.Cookie;

import org.kinslayermud.character.User;
import org.kinslayermud.exception.DataInterfaceException;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;

public class SignInAction extends StandardAction {

  protected String SUCCESS_FORWARD = "Success";
  protected String FAILURE_FORWARD = "Failure";
  
  protected String USERNAME_PARAMETER = "Username";
  protected String PASSWORD_PARAMETER = "Password";
  
  public String execute(WebSupport webSupport) throws Exception {

    List<String> errors = new LinkedList<String>();
    
    String username = request.getParameter(USERNAME_PARAMETER);
    String password = request.getParameter(PASSWORD_PARAMETER);
    
    if(username == null || username.isEmpty()) {
      
      errors.add("You must enter a username.");
    }
    if(password == null || password.isEmpty()) {
      
      errors.add("You must enter a password.");
    }
    
    if(errors.size() > 0) {
      
      request.setAttribute("FormErrors", errors);
      return FAILURE_FORWARD;
    }
    
    User user = webSupport.performLogin(username, password);
    
    if(user == null) {
      
      errors.add("The username or password you entered was incorrect. Please try again.");
      request.setAttribute("FormErrors", errors);
      return FAILURE_FORWARD;
    }
    
    //Login was successful. 
    createUserSession(user);
    return SUCCESS_FORWARD;
  }
  
  public void createUserSession(User user) throws UnsupportedEncodingException, DataInterfaceException {
    
    Cookie cookie = new Cookie("SESSIONUSERID", String.valueOf(user.getUserId()));
    cookie.setMaxAge(90 * 60 * 60 * 24);
    cookie.setPath("/");
    response.addCookie(cookie);
    
    cookie = new Cookie("SESSIONUSERNAME", URLEncoder.encode(user.getUserName(), "UTF-8"));
    cookie.setMaxAge(90 * 60 * 60 * 24);
    cookie.setPath("/");
    response.addCookie(cookie);
    
    String sessionId = MiscUtil.createRandomID(40);
    cookie = new Cookie("SESSIONID", sessionId);
    cookie.setMaxAge(90 * 60 * 60 * 24);
    cookie.setPath("/");
    response.addCookie(cookie);
    
    webSupport.createUserSession(user.getUserId(), sessionId);
  }
}
