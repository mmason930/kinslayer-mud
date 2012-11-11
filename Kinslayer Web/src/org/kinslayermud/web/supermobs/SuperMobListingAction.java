package org.kinslayermud.web.supermobs;

import java.util.List;

import org.kinslayermud.mob.MobPrototype;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.StringUtil;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;

public class SuperMobListingAction extends StandardAction {

  protected String SUCCESS_FORWARD = "Success";
  protected String FAILURE_FORWARD = "Failure";
  
  protected String MOBPROTOTYPEID_PARAMETER = "MobPrototypeId";
  
  public String execute(WebSupport webSupport) throws Exception {

    String mobPrototypeIdParameter = StringUtil.removeNull(request.getParameter(MOBPROTOTYPEID_PARAMETER));
    Integer mobPrototypeId = null;
    
    List<MobPrototype> superMobPrototypes = webSupport.getSuperMobPrototypes();

    if(MiscUtil.isValidIntString(mobPrototypeIdParameter)) {
      
      mobPrototypeId = Integer.valueOf(mobPrototypeIdParameter);
      for(MobPrototype mobPrototype : superMobPrototypes) {
      
        if(mobPrototype.getId() == mobPrototypeId) {
          
          request.setAttribute("MobPrototype", mobPrototype);
        }
      }
    }
    
    request.setAttribute("SuperMobPrototypes", superMobPrototypes);
    
    return SUCCESS_FORWARD;
  }
}
