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
  
  public static List<Achievement> getAchievementsMeetingCriteria(Statement statement, String criteria) throws SQLException {
    
    return QueryUtil.retrieveDataObjectList(statement, "achievement_data", criteria, Achievement.class);
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
