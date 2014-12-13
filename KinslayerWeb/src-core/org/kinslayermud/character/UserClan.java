package org.kinslayermud.character;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Date;

import org.kinslayermud.dbutils.DBObject;
import org.kinslayermud.misc.DataObjectWithIntId;
import org.kinslayermud.util.QueryUtil;

public class UserClan extends DataObjectWithIntId implements DBObject {

  protected int userId;
  protected int clanId;
  protected Date clannedDatetime;
  protected Date lastRankedDatetime;
  protected int questPoints;
  protected int rank;
  protected boolean isCouncil;
  
  public UserClan() {
    
    id = NEW;
  }
  
  public int getUserId() {
    return userId;
  }
  public void setUserId(int userId) {
    this.userId = userId;
  }
  public int getClanId() {
    return clanId;
  }
  public void setClanId(int clanId) {
    this.clanId = clanId;
  }
  public Date getClannedDatetime() {
    return clannedDatetime;
  }
  public void setClannedDatetime(Date clannedDatetime) {
    this.clannedDatetime = clannedDatetime;
  }
  public Date getLastRankedDatetime() {
    return lastRankedDatetime;
  }
  public void setLastRankedDatetime(Date lastRankedDatetime) {
    this.lastRankedDatetime = lastRankedDatetime;
  }
  public int getQuestPoints() {
    return questPoints;
  }
  public void setQuestPoints(int questPoints) {
    this.questPoints = questPoints;
  }
  public int getRank() {
    return rank;
  }
  public void setRank(int rank) {
    this.rank = rank;
  }
  public boolean isCouncil() {
    return isCouncil;
  }
  public void setCouncil(boolean isCouncil) {
    this.isCouncil = isCouncil;
  }
  
  public void retrieveFromResultSet(ResultSet resultSet) throws SQLException {
    
    setId(resultSet.getInt("userClan.id"));
    setUserId(resultSet.getInt("userClan.user_id"));
    setClanId(resultSet.getInt("userClan.clan_id"));
    setClannedDatetime(resultSet.getTimestamp("userClan.clanned_datetime"));
    setLastRankedDatetime(resultSet.getTimestamp("userClan.last_ranked_datetime"));
    setQuestPoints(resultSet.getInt("userClan.quest_points"));
    setRank(resultSet.getInt("userClan.rank"));
    setCouncil(QueryUtil.getIntBoolean(resultSet, "userClan.is_council"));
  }
}
