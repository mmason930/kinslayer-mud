package org.kinslayermud.util;

import java.util.List;

import org.kinslayermud.help.HelpFile;
import org.kinslayermud.object.EquipmentListingType;
import org.kinslayermud.object.ObjectWeaponType;
import org.kinslayermud.object.ObjectWearType;

public class WebSiteUrlUtil {

  public static String getUserLogUrl(String instanceDomain, int userLogId, boolean htmlMode, boolean forumMode) {
    
    String displayType = null;
    
    if(htmlMode)
      displayType = "Html";
    else if(forumMode)
      displayType=  "Forum";
    
    return "http://" + instanceDomain + "/player-portal-view-user-log/" + userLogId + "/" + (displayType == null ? "" : ("?DisplayType=" + displayType));
  }
  
  public static String getEquipmentListingUrl(String instanceDomain, EquipmentListingType equipmentListingType, ObjectWearType objectWearType, ObjectWeaponType objectWeaponType) {
    
    String url = "http://" + instanceDomain + "/equipment-listing";
    
    if(equipmentListingType != null) {
      url += "/" + equipmentListingType.getPluralName().toLowerCase();

      if(equipmentListingType.equals(EquipmentListingType.armor)) {
        
        if(objectWearType != null) {
          
          url += "/" + cleanUrlDisplayText(objectWearType.getDisplayTerm()) + "-" + objectWearType.value();
        }
      }
      else if(equipmentListingType.equals(EquipmentListingType.weapon)) {
        
        if(objectWeaponType != null) {
          
          url += "/" + cleanUrlDisplayText(objectWeaponType.getPluralName()) + "-" + objectWeaponType.value();
        }
      }
    }
    
    return url;
  }
  
  public static String getPlayerPortalSignInUrl() {
    return "./player-portal";
  }
  
  public static String getPlayerPortalSignOutUrl() {
    return "./player-portal-sign-out";
  }
  public static String getPlayerPortalHomeUrl() {
    return "./player-portal-home";
  }
  public static String getPlayerPortalViewTrophiesUrl() {
    return "./player-portal/view-trophies";
  }
  public static String getPlayerPortalViewLoginHistoryUrl() {
    return "./player-portal/view-login-history";
  }
  public static String getPlayerPortalViewTellHistoryUrl(Integer pageNumber) {
    return "./player-portal/view-tell-history" + (pageNumber != null ? ("/" + pageNumber) : "");
  }
  public static String getPlayerPortalViewUserLogs() {
    return "./player-portal/view-user-logs";
  }
  public static String getPlayerPortalHelpEditorUrl() {
    return "./player-portal/help-editor";
  }
  public static String getAdminOlistUrl() {
    return "./admin/olist";
  }
  
  public static String getForumProfileUrl(int userId) {
    return "./forums/memberlist.php?mode=viewprofile&u=" + userId;
  }
  public static String getSuperMobListingUrl(String instanceDomain, int mobPrototypeId, String mobShortDescription) {
    return "http://" + instanceDomain + "/supermob-listing/" + cleanUrlDisplayText(mobShortDescription) + "-" + mobPrototypeId;
  }
  public static String getHelpFileUrl(String instanceDomain, List<HelpFile> helpFileChain) {
    
    StringBuffer urlStringBuffer = new StringBuffer("http://" + instanceDomain + "/help");
    for(HelpFile helpFile : helpFileChain) {
      urlStringBuffer.append("/" + helpFile.getModRewrittenName());
    }
    
    return urlStringBuffer.toString();
  }
  
  public static String getHelpFileUrl(String instanceDomain, List<HelpFile> parentHelpFileChain, HelpFile helpFile) {
    
    StringBuffer urlStringBuffer = new StringBuffer("http://" + instanceDomain + "/help");
    for(HelpFile parentHelpFile : parentHelpFileChain) {
      urlStringBuffer.append("/" + parentHelpFile.getModRewrittenName());
    }
    
    return urlStringBuffer.toString() + "/" + helpFile.getModRewrittenName();
  }
  
  protected static String cleanUrlDisplayText (String urlDisplayText) {

    urlDisplayText = StringUtil.removeNull(urlDisplayText);

    StringBuffer newStr = new StringBuffer("");
    for(int i = 0; i < urlDisplayText.length(); i++) {
      if(
         ((int)urlDisplayText.charAt(i) >= 48 && (int)urlDisplayText.charAt(i) <= 57) || //Numeric
         ((int)urlDisplayText.charAt(i) >= 65 && (int)urlDisplayText.charAt(i) <= 90) || //A-Z
         ((int)urlDisplayText.charAt(i) >= 97 && (int)urlDisplayText.charAt(i) <= 122) || //a-z
         urlDisplayText.charAt(i) == ' ' || 
         urlDisplayText.charAt(i) == '-') {
        
        if(urlDisplayText.charAt(i) == ' ') {

          if(i != urlDisplayText.length() - 1) {

            if(newStr.length() > 0 && newStr.charAt(newStr.length() - 1) != '-') {
              newStr.append("-");
            }
          }
        }
        else if(urlDisplayText.charAt(i) == '-') {

          if(i != urlDisplayText.length() - 1) {

            if(newStr.length() > 0 && newStr.charAt(newStr.length() - 1) != '-') {
              newStr.append("-");
            }
          }
        }
        else {
          newStr.append(urlDisplayText.charAt(i));
        }
      } 
    }

    return newStr.toString().toLowerCase();
  }
  
  public static String getUserProfileUrl(String instanceDomain, String username) {
    
    return "http://" + instanceDomain + "/users/" + username.toLowerCase();
  }
  
  public static String getDefaultUserProfileAvatarUrl(String instanceDomain) {
    
    return "http://" + instanceDomain + "/user-profile/default-avatar.jpg";
  }
  
  public static String getForumAvatarUrl(String instanceDomain, String avatarFileName) {
    
    return "http://" + instanceDomain + "/forums/download/file.php?avatar=" + avatarFileName;
  }
}
