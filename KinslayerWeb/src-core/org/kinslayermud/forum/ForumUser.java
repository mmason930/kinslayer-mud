package org.kinslayermud.forum;

import org.kinslayermud.misc.DataObjectWithIntId;

public class ForumUser extends DataObjectWithIntId {
  
  protected String username;
  protected String avatarUrl;
  
  public String getUsername() {
    return username;
  }
  
  public void setUsername(String username) {
    this.username = username;
  }
  
  public String getAvatarUrl() {
    return avatarUrl;
  }
  
  public void setAvatarUrl(String avatarUrl) {
    this.avatarUrl = avatarUrl;
  }
}
