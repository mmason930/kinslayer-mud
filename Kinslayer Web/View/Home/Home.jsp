<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%

List<PlayerKill> playerKills = (List<PlayerKill>)request.getAttribute("PlayerKills");
Map<Integer, User> userMap = (Map<Integer, User>)request.getAttribute("UserMap");

%>
        <a href="./mudclient/mudclient.php" class="playNowButton">Play Now!</a>

		<div class="battleLogContainer">
			<div class="battleLogDescription">Live Battle Reports</div>
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
    
      killerNamesDisp += killerUser.getUserName();
    
      ++playersDisplayedSoFar;
    }
  }
  
%>
					<span>(<%=timeOfDeathDisp%>) <%= killerNamesDisp %> slew <%=victimUser.getUserName()%> for a gain of <%= totalWeavePointsTransfered %> weave point<%=totalWeavePointsTransfered == 1 ? "" : "s" %>.</span><br/>
<%
}
%>
				</div>
			</div>
		</div>

