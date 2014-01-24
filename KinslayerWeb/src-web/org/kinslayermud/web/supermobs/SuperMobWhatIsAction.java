package org.kinslayermud.web.supermobs;

import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;

public class SuperMobWhatIsAction extends StandardAction {

  protected String SUCCESS_FORWARD = "Success";
  public String execute(WebSupport webSupport) throws Exception {

    return SUCCESS_FORWARD;
  }
}
