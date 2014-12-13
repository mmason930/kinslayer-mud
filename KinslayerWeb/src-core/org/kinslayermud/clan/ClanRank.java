package org.kinslayermud.clan;

import java.sql.ResultSet;
import java.sql.SQLException;

import org.kinslayermud.dbutils.DBObject;
import org.kinslayermud.misc.DataObjectWithIntId;

public class ClanRank extends DataObjectWithIntId implements DBObject {

  protected int clanId;
  protected int rankNumber;
  protected String name;
  
  public ClanRank() {
    
    id = NEW;
  }
  
  public void retrieveFromResultSet(ResultSet resultSet) throws SQLException {
    
    setId(resultSet.getInt("clanRank.id"));
    setClanId(resultSet.getInt("clanRank.clan_id"));
    setRankNumber(resultSet.getInt("clanRank.rank_number"));
    setName(resultSet.getString("clanRank.name"));
  }
  
  public int getClanId() {
    return clanId;
  }
  public void setClanId(int clanId) {
    this.clanId = clanId;
  }
  public int getRankNumber() {
    return rankNumber;
  }
  public void setRankNumber(int rankNumber) {
    this.rankNumber = rankNumber;
  }
  public String getName() {
    return name;
  }
  public void setName(String name) {
    this.name = name;
  }
}
