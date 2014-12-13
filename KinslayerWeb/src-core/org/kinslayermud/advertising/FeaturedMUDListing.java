package org.kinslayermud.advertising;

import java.sql.ResultSet;
import java.sql.SQLException;

import org.kinslayermud.dbutils.DBObject;
import org.kinslayermud.misc.DataObjectWithIntId;

public class FeaturedMUDListing extends DataObjectWithIntId implements DBObject {

  protected String websiteUrl;
  protected String imageUrl;
  protected String caption;
  
  public FeaturedMUDListing() {
    
    id = NEW;
  }

  public String getWebsiteUrl() {
    return websiteUrl;
  }

  public void setWebsiteUrl(String websiteUrl) {
    this.websiteUrl = websiteUrl;
  }

  public String getImageUrl() {
    return imageUrl;
  }

  public void setImageUrl(String imageUrl) {
    this.imageUrl = imageUrl;
  }

  public String getCaption() {
    return caption;
  }

  public void setCaption(String caption) {
    this.caption = caption;
  }
  
  public void retrieveFromResultSet(ResultSet resultSet) throws SQLException {
    
    setId(resultSet.getInt("id"));
    setWebsiteUrl(resultSet.getString("website_url"));
    setImageUrl(resultSet.getString("image_url"));
    setCaption(resultSet.getString("caption"));
  }
}
