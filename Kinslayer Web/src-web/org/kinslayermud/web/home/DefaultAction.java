package org.kinslayermud.web.home;

import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;

public class DefaultAction extends StandardAction {

  private static String REDIRECT_FORWARD = "Redirect";
  private static String FAILURE_FORWARD = "Failure";
  
  public String execute(WebSupport webSupport) {
    
    if(request.getRequestURI().equals("/")) {
      
      return REDIRECT_FORWARD;
    }
    
    return FAILURE_FORWARD;
  }
}
