<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
User user = (User)request.getAttribute("User");
List<String> clanDisplayList = (List<String>)request.getAttribute("ClanDisplayList");
List<String> recentKillsDisplayList = (List<String>)request.getAttribute("RecentKillsDisplayList");
Map<Integer, Achievement> achievementMap = (Map<Integer, Achievement>)request.getAttribute("AchievementMap");
String avatarUrl = (String)request.getAttribute("AvatarURL");
String userClassDisplay = (String)request.getAttribute("UserClassDisplay");
String userRaceDisplay = (String)request.getAttribute("UserRaceDisplay");

String description = user.getDescription().trim();
if(StringUtil.isNullOrEmptyTrimmedString(description)) {
  
  description = user.getUserName() + " does not have a description.";
}
%>

<div class="userProfileContainer">

	<div class="subCategory userSummaryContainer clearFix">
		<div class="usernameAndAvatarContainer">
			<div class="avatar">
<%
if(avatarUrl != null) {
%>
				<img class="avatar" src="<%=avatarUrl%>">
<%
}
%>
			</div>
			<div class="usernameContainer">
				<span class="username"><%=StringUtil.escapeHTMLCharacters(user.getUserName()) %></span><br/>
				<span class="title"><%=StringUtil.escapeHTMLCharacters(StringUtil.removeNull(user.getTitle())) %></span><br>
				<div class="star">
					<img src="./images/star-icon-100x100.png" />
					<span class="level"><%=user.getLevel() %></span>
				</div>
			</div>
		</div>
		
		<div class="descriptionContainer">
			<span class="bigBold">Player's Description</span><br/>
			<div class="playerDescription"><%=StringUtil.escapeHTMLCharacters(description) %></div>
		</div>
	</div>
	
	<div class="subCategory clearFix" id="userDetails">
	
		<div class="userStatistics">
			<div class="bigBold textCenter">User Statistics</div>
			<span class="label">Players Killed: </span><%=MiscUtil.formatNumbericValueWithCommas(user.getPks()) %><br/>
			<span class="label">Mob Deaths: </span><%=MiscUtil.formatNumbericValueWithCommas(user.getMobDeaths()) %><br/>
			<span class="label">Player Deaths: </span><%=MiscUtil.formatNumbericValueWithCommas(user.getPkDeaths()) %><br/>
			<span class="label">Created: </span><%=MiscUtil.formatDateMMMsDDcsYYYY(user.getCreatedDatetime())%><br/>
<%
if(user.getLastLogon() != null) {
%>
			<span class="label">Last Sign In: </span><%=MiscUtil.formatDateMMMsDDcsYYYY(user.getLastLogon())%><br/>
<%
}
if(userRaceDisplay != null) {
%>
			<span class="label">Race: </span><%=StringUtil.escapeHTMLCharacters(userRaceDisplay)%><br/>
<%
}
if(userClassDisplay != null) {
%>
			<span class="label">Class: </span><%=StringUtil.escapeHTMLCharacters(userClassDisplay)%><br/>
<%
}
%>
		</div>
		
		<div class="knownClans">
			<div class="bigBold textCenter">Known Clans</div>
			<ul>
<%
for(String clanDisplay : clanDisplayList) {
%>
				<li><%=StringUtil.escapeHTMLCharacters(clanDisplay) %></li>
<%
}
%>
			</ul>
		</div>
		
		<div class="recentPlayerKills">
			<div class="bigBold textCenter">Recent Player Kills</div>
<%
for(String killDisplay : recentKillsDisplayList) {
%>
            <%= killDisplay %><br/>
<%
}
%>
		</div>
	</div>
	
	<div class="subCategory clearFix" id="userAchievements">
	
		<span class="bigBold header">Achievements</span>
		<div class="achievementContainer">
<%
for(int achievementId : achievementMap.keySet()) {
  
 Achievement achievement = achievementMap.get(achievementId);
%>
			<div class="achievement">
				<img src="./images/achievements/<%=StringUtil.escapeHTMLCharacters(achievement.getImageFileName()) %>" />
				<span><%= StringUtil.escapeHTMLCharacters(achievement.getName()) %></span>
			</div>
<%
}
%>
		</div>
	</div>
</div>
