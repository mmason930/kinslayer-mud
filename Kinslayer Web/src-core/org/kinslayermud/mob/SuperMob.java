package org.kinslayermud.mob;

import org.kinslayermud.misc.DataObjectWithIntId;

public class SuperMob extends DataObjectWithIntId {

  protected int mobId;
  protected SuperMobDifficulty difficulty;
  protected SuperMobStatus status;
  protected String description;
  protected String mobImageUrl;
  protected String mapImageUrl;
  
  public int getMobId() {
    return mobId;
  }
  public void setMobId(int mobId) {
    this.mobId = mobId;
  }
  public SuperMobDifficulty getDifficulty() {
    return difficulty;
  }
  public void setDifficulty(SuperMobDifficulty difficulty) {
    this.difficulty = difficulty;
  }
  public SuperMobStatus getStatus() {
    return status;
  }
  public void setStatus(SuperMobStatus status) {
    this.status = status;
  }
  public String getDescription() {
    return description;
  }
  public void setDescription(String description) {
    this.description = description;
  }
  public String getMobImageUrl() {
    return mobImageUrl;
  }
  public void setMobImageUrl(String mobImageUrl) {
    this.mobImageUrl = mobImageUrl;
  }
  public String getMapImageUrl() {
    return mapImageUrl;
  }
  public void setMapImageUrl(String mapImageUrl) {
    this.mapImageUrl = mapImageUrl;
  }
}
