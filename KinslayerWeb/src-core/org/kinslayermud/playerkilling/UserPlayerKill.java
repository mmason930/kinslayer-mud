package org.kinslayermud.playerkilling;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Date;

import org.kinslayermud.dbutils.DBObject;
import org.kinslayermud.misc.DataObjectWithIntId;

public class UserPlayerKill extends DataObjectWithIntId implements DBObject {

  protected int killId;
  protected int killerId;
  protected int victimId;
  protected int weavePointTransfer;
  protected Date timeOfDeath;

  public int getKillId() {
    return killId;
  }
  public void setKillId(int killId) {
    this.killId = killId;
  }
  public int getKillerId() {
    return killerId;
  }
  public void setKillerId(int killerId) {
    this.killerId = killerId;
  }
  public int getVictimId() {
    return victimId;
  }
  public void setVictimId(int victimId) {
    this.victimId = victimId;
  }
  public int getWeavePointTransfer() {
    return weavePointTransfer;
  }
  public void setWeavePointTransfer(int weavePointTransfer) {
    this.weavePointTransfer = weavePointTransfer;
  }
  public Date getTimeOfDeath() {
    return timeOfDeath;
  }
  public void setTimeOfDeath(Date timeOfDeath) {
    this.timeOfDeath = timeOfDeath;
  }
  
  public void retrieveFromResultSet(ResultSet resultSet) throws SQLException {
    
    setId(resultSet.getInt("id"));
    setKillId(resultSet.getInt("kill_id"));
    setKillerId(resultSet.getInt("killer_id"));
    setVictimId(resultSet.getInt("victim_id"));
    setWeavePointTransfer(resultSet.getInt("wp_transfer"));
    setTimeOfDeath(resultSet.getTimestamp("time_of_death"));
  }
}
