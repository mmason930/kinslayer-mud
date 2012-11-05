package org.kinslayermud.web.newtomuds;

import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StoreAction;

public class NewToMudsHomeAction extends StoreAction {

  protected String SUCCESS_FORWARD = "Success";
  public String execute(WebSupport webSupport) throws Exception {

    return SUCCESS_FORWARD;
  }
}
