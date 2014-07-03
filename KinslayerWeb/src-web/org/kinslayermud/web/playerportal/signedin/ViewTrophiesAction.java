package org.kinslayermud.web.playerportal.signedin;

import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashSet;
import java.util.List;
import java.util.Map;

import org.kinslayermud.character.User;
import org.kinslayermud.playerkilling.PlayerKill;
import org.kinslayermud.util.WebSupport;

public class ViewTrophiesAction extends ValidateSignInAction {

  protected String SUCCESS_FORWARD = "Success";
  protected String FAILURE_FORWARD = "Failure";
  
  public String execute(WebSupport webSupport, User user) throws Exception {
    
    List<PlayerKill> playerKills = webSupport.getPlayerKillsByKillerId(user.getUserId(), null, null, true);
    Collection<Integer> userIdCollection = new HashSet<Integer>();
    Map<Integer, User> userMap;
    
    for(PlayerKill playerKill : playerKills) {
      
      userIdCollection.addAll(playerKill.getUserIdSet());
    }
    
    Collections.sort(playerKills, new Comparator<PlayerKill>() {

      public int compare(PlayerKill o1, PlayerKill o2) {
        return o2.getTimeOfDeath().compareTo(o1.getTimeOfDeath());
      }
    });
    
    userMap = webSupport.getUserMap(userIdCollection);
    
    request.setAttribute("PlayerKills", playerKills);
    request.setAttribute("UserMap", userMap);
    request.setAttribute("User", user);
    
    return SUCCESS_FORWARD;
  }
}
