package org.kinslayermud.web.equipment;

import org.kinslayermud.object.ObjectWearType;
import org.kinslayermud.object.WeaponType;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;

public class EquipmentCatalogAction extends StandardAction {

  protected String SUCCESS_FORWARD = "Success";
  public String execute(WebSupport webSupport) throws Exception {

    return SUCCESS_FORWARD;
  }
  
  public static class ObjectCategory {
    
    protected String name;
    protected ObjectWearType objectWearType;
    protected WeaponType weaponType;
  }
}
