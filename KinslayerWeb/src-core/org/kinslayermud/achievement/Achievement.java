package org.kinslayermud.achievement;

import org.kinslayermud.misc.DataObjectWithIntId;

public class Achievement extends DataObjectWithIntId {

  protected boolean hidden;
  protected String name;
  protected String description;
  protected int featPoints;
  protected boolean alwaysAward;
  protected String imageFileName;
  
  public boolean isHidden() {
    return hidden;
  }
  public void setHidden(boolean hidden) {
    this.hidden = hidden;
  }
  public String getName() {
    return name;
  }
  public void setName(String name) {
    this.name = name;
  }
  public String getDescription() {
    return description;
  }
  public void setDescription(String description) {
    this.description = description;
  }
  public int getFeatPoints() {
    return featPoints;
  }
  public void setFeatPoints(int featPoints) {
    this.featPoints = featPoints;
  }
  public boolean isAlwaysAward() {
    return alwaysAward;
  }
  public void setAlwaysAward(boolean alwaysAward) {
    this.alwaysAward = alwaysAward;
  }
  public String getImageFileName() {
    return imageFileName;
  }
  public void setImageFileName(String imageFileName) {
    this.imageFileName = imageFileName;
  }
}