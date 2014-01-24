package org.kinslayermud.web.help;

import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;

public class HomeAction extends StandardAction {

  protected static final String SUCCESS_FORWARD = "Success";
  
  public String execute(WebSupport webSupport) throws Exception {
    
    return SUCCESS_FORWARD;
  }
}
