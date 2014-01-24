<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
User user = (User)request.getAttribute("User");
List<PlayerKill> playerKills = (List<PlayerKill>)request.getAttribute("PlayerKills");
Map<Integer, User> userMap = (Map<Integer, User>)request.getAttribute("UserMap");
%>
<table class="userPortalTrophies">
	<tr>
		<th class="victimName">Victim Name</th>
		<th class="time">Time</th>
		<th class="weavePoints">Weave Pts</th>
		<th class="partyMembers">Party Members</th>
	</tr>
<%
for(PlayerKill playerKill : playerKills) {

  User victimUser = userMap.get(playerKill.getVictimUserId());
  String victimForumProfileUrl = null;
  if(victimUser != null) {
    victimForumProfileUrl = WebSiteUrlUtil.getForumProfileUrl(playerKill.getVictimUserId());
  }
  String timeOfDeath = MiscUtil.formatDateMMMsDDcsYYYYsHHsMMsSS(playerKill.getTimeOfDeath());
  UserPlayerKill userPlayerKill = playerKill.getUserPlayerKillByKillerUserId(user.getUserId());
  List<String> partyMembers = new LinkedList<String>();
  
  for(UserPlayerKill partyMemberUserPlayerKill : playerKill.getUserPlayerKills()) {
    
    User partyMemberUser = userMap.get(partyMemberUserPlayerKill.getKillerId());
    if(partyMemberUser != null) {
      int partyMemberUserId = partyMemberUser.getUserId();
      String partyMemberUserName = partyMemberUser.getUserName();
      partyMembers.add("<a href='" + WebSiteUrlUtil.getForumProfileUrl(partyMemberUserId) + "'>" + partyMemberUserName + "</a>");
    }
  }
%>
	<tr>
		<td>
<%
  if(victimUser != null) {
%>
			<a href="<%=victimForumProfileUrl%>">
<%
  }
%>
				<%=victimUser == null ? "&lt;Deleted User&gt;" : victimUser.getUserName()%>
<%
  if(victimUser != null) {
%>
			</a>
<%
  }
%>
		</td>
		<td><%=timeOfDeath %></td>
		<td><%= userPlayerKill.getWeavePointTransfer()%></td>
		<td><%=StringUtil.buildStringFromList(partyMembers, ", ") %></td>
	</tr>
<%
}
%>
</table>