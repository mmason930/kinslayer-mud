package org.kinslayermud.web.home;

import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StoreAction;

public class DefaultAction extends StoreAction {

  private static String REDIRECT_FORWARD = "Redirect";
  private static String FAILURE_FORWARD = "Failure";
  
  public String execute(WebSupport webSupport) {
    
    System.out.println("Request URI: " + request.getRequestURI());
    if(request.getRequestURI().equals("/")) {
      
      return REDIRECT_FORWARD;
    }
    
    return FAILURE_FORWARD;
  }
}
