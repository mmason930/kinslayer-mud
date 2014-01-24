package org.kinslayermud.web.supermobs;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletResponse;

import org.kinslayermud.kit.KitWithItemsAndObjectPrototypes;
import org.kinslayermud.mob.MobPrototype;
import org.kinslayermud.mob.SuperMob;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.StringUtil;
import org.kinslayermud.util.WebSiteUrlUtil;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;
import org.kinslayermud.zone.Zone;

public class SuperMobListingAction extends StandardAction {

  protected String SUCCESS_FORWARD = "Success";
  protected String FAILURE_FORWARD = "Failure";
  
  protected String MOBPROTOTYPEID_PARAMETER = "MobPrototypeId";
  protected String MOBPROTOTYPEIDNEW_PARAMETER = "MobPrototypeIdNew";
  
  public String execute(WebSupport webSupport) throws Exception {

    String mobPrototypeIdParameter = StringUtil.removeNull(request.getParameter(MOBPROTOTYPEIDNEW_PARAMETER));
    Integer mobPrototypeId = null;
    
    //301 redirect all requests sent to the old URL format.
    if(request.getParameter(MOBPROTOTYPEID_PARAMETER) != null) {
      
      mobPrototypeId = Integer.valueOf(request.getParameter(MOBPROTOTYPEID_PARAMETER));
      MobPrototype mobPrototype = webSupport.getMobPrototype(mobPrototypeId);
      
      response.setStatus(HttpServletResponse.SC_MOVED_PERMANENTLY);
      response.setHeader("Location", WebSiteUrlUtil.getSuperMobListingUrl(webSupport.getInstanceDomain(), mobPrototypeId, mobPrototype.getShortDescription()));
      return null;
    }

    List<SuperMob> superMobs = webSupport.getAllOpenSuperMobs();
    Collection<Integer> mobPrototypeIdCollection = new HashSet<Integer>();
    Map<Integer, MobPrototype> mobPrototypeMap = new HashMap<Integer, MobPrototype>();
    
    for(SuperMob superMob : superMobs) {
      
      mobPrototypeIdCollection.add(superMob.getMobId());
    }

    mobPrototypeMap = webSupport.getMobPrototypeMap(mobPrototypeIdCollection);
    MobPrototype mobPrototype = null;
    if(MiscUtil.isValidIntString(mobPrototypeIdParameter)) {
      
      mobPrototypeId = Integer.valueOf(mobPrototypeIdParameter);
      for(SuperMob superMob : superMobs) {
      
        if(superMob.getMobId() == mobPrototypeId) {

          mobPrototype = mobPrototypeMap.get(superMob.getMobId());
          Zone zone = webSupport.getZoneLoadingSuperMobPrototype(superMob.getMobId());
          KitWithItemsAndObjectPrototypes kitWithItemsAndObjectPrototypes = webSupport.getKitWithItemsAndObjectPrototypes(mobPrototype.getPrimaryKit());
          
          request.setAttribute("Zone", zone);
          request.setAttribute("MobPrototype", mobPrototype);
          request.setAttribute("SuperMob", superMob);
          request.setAttribute("KitWithItemsAndObjectPrototypes", kitWithItemsAndObjectPrototypes);
          break;
        }
      }
    }
    
    if(mobPrototype != null) {
      
      request.setAttribute("MetaDescription", mobPrototype.getShortDescription().replace("\"", "") + " Super Mob listing. View other super mobs in the game.");
      request.setAttribute("Title", mobPrototype.getShortDescription().replace("\"", "") + " | Super MOB Listing | Kinslayer MUD");
    }
    
    request.setAttribute("SuperMobs", superMobs);
    request.setAttribute("MobPrototypeMap", mobPrototypeMap);
    
    return SUCCESS_FORWARD;
  }
}
