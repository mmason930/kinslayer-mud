package org.kinslayermud.advertising;

import org.kinslayermud.misc.DataObjectWithIntId;

public class FeaturedMUDListing extends DataObjectWithIntId {

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
}
