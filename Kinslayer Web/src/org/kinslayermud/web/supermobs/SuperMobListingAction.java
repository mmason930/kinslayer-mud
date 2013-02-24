package org.kinslayermud.web.supermobs;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;

import org.kinslayermud.kit.KitWithItemsAndObjectPrototypes;
import org.kinslayermud.mob.MobPrototype;
import org.kinslayermud.mob.SuperMob;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.StringUtil;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;
import org.kinslayermud.zone.Zone;

public class SuperMobListingAction extends StandardAction {

  protected String SUCCESS_FORWARD = "Success";
  protected String FAILURE_FORWARD = "Failure";
  
  protected String MOBPROTOTYPEID_PARAMETER = "MobPrototypeId";
  
  public String execute(WebSupport webSupport) throws Exception {

    System.out.println("Supermob listing.");
    String mobPrototypeIdParameter = StringUtil.removeNull(request.getParameter(MOBPROTOTYPEID_PARAMETER));
    Integer mobPrototypeId = null;

    List<SuperMob> superMobs = webSupport.getAllOpenSuperMobs();
    Collection<Integer> mobPrototypeIdCollection = new HashSet<Integer>();
    Map<Integer, MobPrototype> mobPrototypeMap = new HashMap<Integer, MobPrototype>();
    
    for(SuperMob superMob : superMobs) {
      
      mobPrototypeIdCollection.add(superMob.getMobId());
    }

    mobPrototypeMap = webSupport.getMobPrototypeMap(mobPrototypeIdCollection);
    
    if(MiscUtil.isValidIntString(mobPrototypeIdParameter)) {
      
      mobPrototypeId = Integer.valueOf(mobPrototypeIdParameter);
      for(SuperMob superMob : superMobs) {
      
        if(superMob.getMobId() == mobPrototypeId) {

          MobPrototype mobPrototype = mobPrototypeMap.get(superMob.getMobId());
          Zone zone = webSupport.getZoneLoadingSuperMobPrototype(superMob.getMobId());
          KitWithItemsAndObjectPrototypes kitWithItemsAndObjectPrototypes = webSupport.getKitWithItemsAndObjectPrototypes(mobPrototype.getPrimaryKit());
          
          request.setAttribute("Zone", zone);
          request.setAttribute("MobPrototype", mobPrototype);
          request.setAttribute("SuperMob", superMob);
          request.setAttribute("KitWithItemsAndObjectPrototypes", kitWithItemsAndObjectPrototypes);
        }
      }
    }
    
    request.setAttribute("SuperMobs", superMobs);
    request.setAttribute("MobPrototypeMap", mobPrototypeMap);
    
    return SUCCESS_FORWARD;
  }
}
