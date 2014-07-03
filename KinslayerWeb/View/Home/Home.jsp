<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%

List<PlayerKill> playerKills = (List<PlayerKill>)request.getAttribute("PlayerKills");
Map<Integer, User> userMap = (Map<Integer, User>)request.getAttribute("UserMap");
Map<String, Obj> objectMap = (Map<String, Obj>)request.getAttribute("ObjectMap");
List<AuctionItem> auctionItems = (List<AuctionItem>)request.getAttribute("AuctionItems");
Map<Integer, AuctionBid> auctionItemIdToHighestAuctionBidMap = (Map<Integer, AuctionBid>)request.getAttribute("AuctionItemIdToHighestAuctionBidMap");

%>
        <a href="./mudclient/mudclient.php" class="playNowButton">Play Now!</a>

		<div class="auctionsContainer">

			<div class="bold centerText">Open Auctions</div><br>
<%
for(AuctionItem auctionItem : auctionItems) {
  Obj object = objectMap.get(auctionItem.getObjectId());
  User user = userMap.get(auctionItem.getOwnerId());
  
  if(object != null) {

    AuctionBid highestAuctionBid = auctionItemIdToHighestAuctionBidMap.get(auctionItem.getId());
    MoneyBreakdown moneyBreakdown = new MoneyBreakdown(highestAuctionBid == null ? auctionItem.getStartingPrice() : highestAuctionBid.getBidAmount());
    String shortDescription = object.getShortDescription();
    String userName = "<Deleted>", userUrl = null;
    
    if(user != null) {
      userName = user.getUserName();
      userUrl = WebSiteUrlUtil.getUserProfileUrl(webSupport.getInstanceDomain(), user.getUserName());
    }
    
%>
			<div class="auction">
				<%=StringUtil.escapeHTMLCharacters(shortDescription) %><br>
				by 
<%
    if(user != null) {
%>
				<a href="<%=userUrl%>" class="blueLink">
<%
    }
%>
					<%=userName %>
<%
    if(user != null) {
%>
				</a>
<%
    }
%>
				<span class="gold"><%=moneyBreakdown.getGold() %>g</span>, 
				<span class="silver"><%=moneyBreakdown.getSilver() %>s</span>,
				<span class="copper"><%=moneyBreakdown.getCopper() %>c</span>
			</div>
<%
  }
}
%>
		</div>

		<div class="homeContent">
			<h1>A Free Online Wheel of Time Based Game</h1>
			<div class="siteDescription">
				<p>Kinslayer MUD is a <strong>free online text-based game</strong>. A Multi-user dungeon is one of the oldest forms of online gaming. They lack the complexity of graphics, allowing for intense competition in ways that no modern-day game can accurately capture.</p>
				<p>Based on Robert Jordan's popular "Wheel of Time" fantasy series, Kinslayer features popular elements of the books such as <strong>Shadowspawn</strong> classes such as <strong>Trollocs</strong> and <strong>Myrddraal</strong> and classes such as tainted male channelers. To jump into the thick of the action, click the "Play Now!" button above!
			</div>
		</div>
		

		<div class="battleLogContainer">
			<h2 class="battleLogDescription">Live Battle Reports</h2>
			<div class="battleLogPrompt">
				<div>
<%
for(PlayerKill playerKill : playerKills) {

  User victimUser = userMap.get(playerKill.getVictimUserId());
  String timeOfDeathDisp = MiscUtil.formatDateHHcMM(playerKill.getTimeOfDeath());
  String killerNamesDisp = "";
  int totalWeavePointsTransfered = playerKill.getTotalWeavePointTransfer();
  
  int playersDisplayedSoFar = 0;
  for(UserPlayerKill userPlayerKill : playerKill.getUserPlayerKills()) {

    User killerUser = userMap.get(userPlayerKill.getKillerId());
    
    if(playersDisplayedSoFar >= 2) {
   
      int remainingToDisplay = playerKill.getUserPlayerKills().size() - playersDisplayedSoFar;
      
      killerNamesDisp += " and " + remainingToDisplay + " other" + (remainingToDisplay == 1 ? "" : "s");
      break;
    }

    if(killerUser != null) {
    
      if(playerKill.getUserPlayerKills().size() > 1 && playersDisplayedSoFar + 1 == playerKill.getUserPlayerKills().size()) {
        killerNamesDisp += " and ";
      }
      else if(playersDisplayedSoFar > 0) {
        killerNamesDisp += ", ";
      }
    
      killerNamesDisp += "<a href='" + WebSiteUrlUtil.getUserProfileUrl(webSupport.getInstanceDomain(), killerUser.getUserName()) + "'>" + killerUser.getUserName() + "</a>";
    
      ++playersDisplayedSoFar;
    }
  }
  
%>
					<span>(<%=timeOfDeathDisp%>) <%= killerNamesDisp %> slew <%="<a href='" + WebSiteUrlUtil.getUserProfileUrl(webSupport.getInstanceDomain(), victimUser.getUserName()) + "'>" + victimUser.getUserName() + "</a>"%> for a gain of <%= totalWeavePointsTransfered %> weave point<%=totalWeavePointsTransfered == 1 ? "" : "s" %>.</span><br/>
<%
}
%>
				</div>
			</div>
		</div>

