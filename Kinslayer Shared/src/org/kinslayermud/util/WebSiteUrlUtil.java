package org.kinslayermud.util;

public class WebSiteUrlUtil {

  public static String getUserLogUrl(String instanceDomain, int userLogId, boolean htmlMode, boolean forumMode) {
    
    String displayType = null;
    
    if(htmlMode)
      displayType = "Html";
    else if(forumMode)
      displayType=  "Forum";
    
    return "http://" + instanceDomain + "/player-portal-view-user-log/" + userLogId + "/" + (displayType == null ? "" : ("?DisplayType=" + displayType));
  }
}
