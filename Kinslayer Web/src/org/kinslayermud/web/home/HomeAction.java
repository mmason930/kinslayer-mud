package org.kinslayermud.web.home;

import java.util.List;
import java.util.Map;

import org.kinslayermud.character.User;
import org.kinslayermud.playerkilling.PlayerKill;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;

public class HomeAction extends StandardAction {

  private static String SUCCESS_FORWARD = "Success";
  
  public String execute(WebSupport webSupport) throws Exception {

    List<PlayerKill> playerKills = webSupport.getHomePlayerKills();
    Map<Integer, User> userMap = webSupport.getHomeUserMap();
    
    /***
    //Auction Items...
    List<AuctionItem> auctionItems = webSupport.getActiveAuctionItems();
    Set<String> objectIdSet = new HashSet<String>();
    
    for(AuctionItem auctionItem : auctionItems) {
      
      objectIdSet.add(auctionItem.getObjectId());
    }
    
    Map<String, Obj> objectMap = webSupport.getObjectMap(objectIdSet);
    ***/
    
    request.setAttribute("PlayerKills", playerKills);
    //request.setAttribute("AuctionItems", auctionItems);
    request.setAttribute("UserMap", userMap);
    
    return SUCCESS_FORWARD;
  }
}
