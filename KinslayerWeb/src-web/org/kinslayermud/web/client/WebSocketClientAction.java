package org.kinslayermud.web.client;

import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;

public class WebSocketClientAction extends StandardAction {

  protected String SUCCESS_FORWARD = "Success";
  public String execute(WebSupport webSupport) throws Exception {

    return SUCCESS_FORWARD;
  }
}
