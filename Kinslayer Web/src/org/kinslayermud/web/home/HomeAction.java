package org.kinslayermud.web.home;

import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Map;

import org.kinslayermud.character.User;
import org.kinslayermud.playerkilling.PlayerKill;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;

public class HomeAction extends StandardAction {

  private static String SUCCESS_FORWARD = "Success";
  
  public String execute(WebSupport webSupport) throws Exception {
    
    List<PlayerKill> playerKills = webSupport.getLastSoManyPlayerKills(10);
    
    Collection<Integer> userIdCollection = new HashSet<Integer>();
    
    for(PlayerKill playerKill : playerKills) {
      
      userIdCollection.addAll(playerKill.getUserIdSet());
    }
    
    Map<Integer, User> userMap = webSupport.getUserMap(userIdCollection);
    
    request.setAttribute("PlayerKills", playerKills);
    request.setAttribute("UserMap", userMap);
    
    return SUCCESS_FORWARD;
  }
}
