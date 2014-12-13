package org.kinslayermud.web.users;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.kinslayermud.achievement.Achievement;
import org.kinslayermud.character.User;
import org.kinslayermud.character.UserClan;
import org.kinslayermud.character.UserPreference;
import org.kinslayermud.clan.Clan;
import org.kinslayermud.clan.ClanRank;
import org.kinslayermud.clan.ClanWithRanks;
import org.kinslayermud.exception.DataInterfaceException;
import org.kinslayermud.forum.ForumUser;
import org.kinslayermud.playerkilling.PlayerKill;
import org.kinslayermud.util.DateUtil;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.StringUtil;
import org.kinslayermud.util.WebSiteUrlUtil;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;

public class ViewProfileAction extends StandardAction {

  protected String SUCCESS_FORWARD = "Success";
  protected static final String USER_NOT_FOUND_FORWARD = "UserNotFound";
  protected static final String USERNAME_PARAMETER = "Username";
  protected static final int PLAYER_KILL_FETCH_SIZE = 10;
  
  public String execute(WebSupport webSupport) throws Exception {

    String username = request.getParameter(USERNAME_PARAMETER);
    
    if(StringUtil.isNullOrEmptyTrimmedString(username)) {
      
      System.out.println("Not Found Forward");
      return USER_NOT_FOUND_FORWARD;
    }
    
    User user = webSupport.getUserByUsername(username);
    
    if(user == null) {
      
      System.out.println("No user by username found `" + username + "`");
      return USER_NOT_FOUND_FORWARD;
    }
    
    List<String> clanDisplayList = getClanDisplayList(user);
    List<String> recentKillsDisplayList = getRecentKillsDisplayList(user.getUserId());
    Map<Integer, Achievement> achievementMap = getAchievementMap(user.getUserId());
    ForumUser forumUser = webSupport.getForumUser(user.getUserId());
    
    String avatarUrl = getAvatarUrl(forumUser);
    String userClassDisplay = getUserClassDisplay(user);
    String userRaceDisplay = getUserRaceDisplay(user);
    
    request.setAttribute("UserClassDisplay", userClassDisplay);
    request.setAttribute("UserRaceDisplay", userRaceDisplay);
    request.setAttribute("AvatarURL", avatarUrl);
    request.setAttribute("Title", "View Profile - " + user.getUserName() + " | KinslayerMUD");
    request.setAttribute("User", user);
    request.setAttribute("ClanDisplayList", clanDisplayList);
    request.setAttribute("RecentKillsDisplayList", recentKillsDisplayList);
    request.setAttribute("AchievementMap", achievementMap);
    
    return SUCCESS_FORWARD;
  }
  
  protected String getUserClassDisplay(User user) {
    
    if(user.preferenceFlagged(UserPreference.incognito)) {
      
      return "Unknown";
    }
    
    if(user.getUserClass().isRemort()) {
      
      return null;
    }
    
    return user.getUserClass().getStandardName();
  }
  
  protected String getUserRaceDisplay(User user) {
    
    if(user.getUserClass().isRemort()) {
      
      return user.getUserClass().getStandardName();
    }
    
    return user.getUserRace().getStandardName();
  }
  
  protected String getAvatarUrl(ForumUser forumUser) {
    
    if(forumUser == null || StringUtil.isNullOrEmptyTrimmedString(forumUser.getAvatarUrl())) {
      
      return null;
      //return WebSiteUrlUtil.getDefaultUserProfileAvatarUrl(webSupport.getInstanceDomain());
    }
    
    return WebSiteUrlUtil.getForumAvatarUrl(webSupport.getInstanceDomain(), forumUser.getAvatarUrl());
  }
  
  protected Map<Integer, Achievement> getAchievementMap(int userId) throws DataInterfaceException {
    
    Map<Integer, Achievement> achievementMap;
    List<Integer> achievementIds = webSupport.getAchievementIdsByUser(userId);
    
    if(achievementIds.isEmpty()) {
      
      achievementMap = new HashMap<Integer, Achievement>();
    }
    else {
      
      achievementMap = webSupport.getAchievementMap(achievementIds);
    }
    
    return achievementMap;
  }
  
  protected String getUserNameWrappedInAnchor(User user) {
    
    if(user == null) {
      
      return StringUtil.escapeHTMLCharacters("<Deleted Player>");
    }
    
    return "<a href='" + WebSiteUrlUtil.getUserProfileUrl(webSupport.getInstanceDomain(), user.getUserName()) + "'>" + StringUtil.escapeHTMLCharacters(user.getUserName()) + "</a>";
  }
  
  protected List<String> getRecentKillsDisplayList(int userId) throws DataInterfaceException {
    
    List<PlayerKill> playerKills = webSupport.getPlayerKillsByKillerId(userId, 0, PLAYER_KILL_FETCH_SIZE, true);
    Collection<Integer> userIdCollection = new HashSet<Integer>();
    List<String> recentKillsDisplayList = new ArrayList<String>();
    
    for(PlayerKill playerKill : playerKills) {
      
      userIdCollection.add(playerKill.getVictimUserId());
    }
    
    Map<Integer, User> victimUserMap = webSupport.getUserMap(userIdCollection);
    
    Collections.sort(playerKills, new Comparator<PlayerKill>() {
      
      public int compare(PlayerKill playerKill1, PlayerKill playerKill2) {
        return playerKill2.getTimeOfDeath().compareTo(playerKill1.getTimeOfDeath());
      }
    });
    
    for(PlayerKill playerKill : playerKills) {
      
      Date startOfToday = DateUtil.startOfDay(new Date());
      Date startOfDayOfDeath = DateUtil.startOfDay(playerKill.getTimeOfDeath());
      User victimUser = victimUserMap.get(playerKill.getVictimUserId());
      
      long daysSinceKill = DateUtil.daysBetween(startOfDayOfDeath, startOfToday);
      String username = getUserNameWrappedInAnchor(victimUser);
      long otherPlayers = playerKill.getKillerUserIdSet().size() - 1;
      
      String withOtherPlayersMessage = otherPlayers == 0 ? "" : " with " + otherPlayers + " other" + StringUtil.plural(otherPlayers);
      
      recentKillsDisplayList.add(
          "Slew " + username + " " + MiscUtil.formatNumbericValueWithCommas(daysSinceKill) + " day" + StringUtil.plural(daysSinceKill) + " ago" + withOtherPlayersMessage + "."
      );
    }
    
    return recentKillsDisplayList;
  }
  
  protected List<String> getClanDisplayList(User user) throws DataInterfaceException {
    
    Map<Integer, ClanWithRanks> clanWithRanksMap;
    Set<Integer> clanIdSet = new HashSet<Integer>();
    List<String> clanDisplayList;
    
    if(!user.preferenceFlagged(UserPreference.incognito)) {
      
      List<UserClan> userClans = webSupport.getUserClansByUserId(user.getUserId());
      
      for(UserClan userClan : userClans) {
        
        clanIdSet.add(userClan.getClanId());
      }
      
      if(clanIdSet.isEmpty()) {
        
        clanWithRanksMap = new HashMap<Integer, ClanWithRanks>();
      }
      else {
        
        clanWithRanksMap = webSupport.getClansWithRanksMap(clanIdSet);
      }

      clanDisplayList = getClanDisplayList(userClans, clanWithRanksMap);
    }
    else {
      
      clanDisplayList = new ArrayList<String>();
    }
    
    if(clanDisplayList.isEmpty()) {
      
      clanDisplayList.add(user.getUserName() + " is not known to belong to any clans.");
    }
    
    return clanDisplayList;
  }
  
  protected List<String> getClanDisplayList(List<UserClan> userClans, Map<Integer, ClanWithRanks> clanWithRanksMap) {
    
    List<String> clanDisplayList = new ArrayList<String>();
    
    for(UserClan userClan : userClans) {
      
      ClanWithRanks clanWithRanks = clanWithRanksMap.get(userClan.getClanId());
      Clan clan = clanWithRanks.getClan();
      ClanRank clanRank = clanWithRanks.getClanRank(userClan.getRank());
      
      if(clanRank == null) {
        
        System.out.println("ERROR: Invalid rank for user #" + userClan.getUserId() + ", clan #" + userClan.getId() + ": " + userClan.getRank());
        continue;
      }
      
      if(clan.isSecret()) {
        
        continue;
      }
      
      clanDisplayList.add(clan.getName() + " " + clanRank.getName() + (userClan.isCouncil() ? " Councilman" : "") + " (Rank " + userClan.getRank() + ")");
    }
    
    return clanDisplayList;
  }
}
