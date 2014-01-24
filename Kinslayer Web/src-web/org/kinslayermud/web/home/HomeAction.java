package org.kinslayermud.web.home;

import java.util.List;
import java.util.Map;

import org.kinslayermud.auction.AuctionBid;
import org.kinslayermud.auction.AuctionItem;
import org.kinslayermud.character.User;
import org.kinslayermud.object.Obj;
import org.kinslayermud.playerkilling.PlayerKill;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;

public class HomeAction extends StandardAction {

  private static String SUCCESS_FORWARD = "Success";
  
  public String execute(WebSupport webSupport) throws Exception {

    List<PlayerKill> playerKills = webSupport.getHomePlayerKills();
    Map<Integer, User> userMap = webSupport.getHomeUserMap();
    Map<String, Obj> objectMap = webSupport.getHomeObjectMap();
    List<AuctionItem> auctionItems = webSupport.getHomeAuctionItems();
    Map<Integer, AuctionBid> auctionItemIdToHighestAuctionBidMap = webSupport.getHomeAuctionItemIdToHighestAuctionBidMap();
    
    request.setAttribute("PlayerKills", playerKills);
    request.setAttribute("AuctionItems", auctionItems);
    request.setAttribute("ObjectMap", objectMap);
    request.setAttribute("UserMap", userMap);
    request.setAttribute("AuctionItemIdToHighestAuctionBidMap", auctionItemIdToHighestAuctionBidMap);
    
    return SUCCESS_FORWARD;
  }
}
