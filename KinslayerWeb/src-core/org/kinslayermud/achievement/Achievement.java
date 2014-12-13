package org.kinslayermud.achievement;

import java.sql.ResultSet;
import java.sql.SQLException;

import org.kinslayermud.dbutils.DBObject;
import org.kinslayermud.misc.DataObjectWithIntId;
import org.kinslayermud.util.QueryUtil;

public class Achievement extends DataObjectWithIntId implements DBObject {

  protected boolean hidden;
  protected String name;
  protected String description;
  protected int featPoints;
  protected boolean alwaysAward;
  protected String imageFileName;
  
  public void retrieveFromResultSet(ResultSet resultSet) throws SQLException {
    
    setId(resultSet.getInt("vnum"));
    setHidden(QueryUtil.getIntBoolean(resultSet, "hidden"));
    setName(resultSet.getString("name"));
    setDescription(resultSet.getString("description"));
    setFeatPoints(resultSet.getInt("feat_points"));
    setAlwaysAward(QueryUtil.getIntBoolean(resultSet, "always_award"));
    setImageFileName(resultSet.getString("image_file_name"));
  }
  
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