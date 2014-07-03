package org.kinslayermud.util;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.kinslayermud.achievement.Achievement;

public class AchievementUtil {

  public static Achievement getAchievement(ResultSet resultSet) throws SQLException {
    
    Achievement achievement = new Achievement();

    achievement.setId(resultSet.getInt("vnum"));
    achievement.setHidden(QueryUtil.getIntBoolean(resultSet, "hidden"));
    achievement.setName(resultSet.getString("name"));
    achievement.setDescription(resultSet.getString("description"));
    achievement.setFeatPoints(resultSet.getInt("feat_points"));
    achievement.setAlwaysAward(QueryUtil.getIntBoolean(resultSet, "always_award"));
    achievement.setImageFileName(resultSet.getString("image_file_name"));
    
    return achievement;
  }
  
  public static List<Achievement> getAchievementsMeetingCriteria(Statement statement, String criteria) throws SQLException {
    
    List<Achievement> achievements = new ArrayList<Achievement>();
    
    String sql = " SELECT *"
               + " FROm achievement_data"
               + " WHERE " + criteria;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      achievements.add(getAchievement(resultSet));
    }
    
    resultSet.close();
    
    return achievements;
  }
  
  public static Map<Integer, Achievement> getAchievementMap(Collection<Achievement> achievements) {
    
    Map<Integer, Achievement> achievementMap = new HashMap<Integer, Achievement>();
    
    for(Achievement achievement : achievements) {
      
      achievementMap.put(achievement.getId(), achievement);
    }
    
    return achievementMap;
  }
  
  public static Map<Integer, Achievement> getAchievementMap(Statement statement, Collection<Integer> achievementIdCollection) throws SQLException {
    
    String criteria = " vnum IN " + SQLUtil.buildListSQL(achievementIdCollection, false, true);
    return getAchievementMap(getAchievementsMeetingCriteria(statement, criteria));
  }
}
