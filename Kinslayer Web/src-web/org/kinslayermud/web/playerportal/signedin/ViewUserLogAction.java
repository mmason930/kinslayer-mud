package org.kinslayermud.web.playerportal.signedin;

import org.kinslayermud.character.User;
import org.kinslayermud.telnet.TelnetColor;
import org.kinslayermud.userlog.UserLog;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.StringUtil;
import org.kinslayermud.util.WebSupport;

public class ViewUserLogAction extends ValidateSignInAction {

  protected String SUCCESS_FORWARD = "Success";
  protected String FAILURE_FORWARD = "Failure";
  
  protected String USERLOGID_PARAMETER = "UserLogId";
  protected String DISPLAYTYPE_PARAMETER = "DisplayType";
  
  public String execute(WebSupport webSupport, User user) throws Exception {
    
    String userLogIdString = request.getParameter(USERLOGID_PARAMETER);
    String displayType = StringUtil.removeNull(request.getParameter(DISPLAYTYPE_PARAMETER));
    if(userLogIdString != null && MiscUtil.isValidIntString(userLogIdString)) {
      
      UserLog userLog = webSupport.getUserLog(Integer.valueOf(userLogIdString));
      
      if(userLog.getUserId() == user.getUserId()) {
        
        boolean forumMode = false, htmlMode = false;
        
        if(displayType.equals("Forum"))
          forumMode = true;
        else if(displayType.equals("Html"))
          htmlMode = true;
        
        userLog.setConsoleOutput(formatUserLog(StringUtil.ConvertToHTML(userLog.getConsoleOutput()), htmlMode, forumMode));
        request.setAttribute("UserLog", userLog);
        return SUCCESS_FORWARD;
      }
    }
    
    return FAILURE_FORWARD;
  }
  
  public static String formatUserLog(String consoleBuffer, boolean htmlMode, boolean forumMode) {
    
    StringBuffer formattedLog = new StringBuffer();
    StringBuffer numberTextStringBuffer = new StringBuffer();
    int tempIndex;
    TelnetColor currentColor = TelnetColor.normal;
    boolean isBolded = false;
    
    if(forumMode) {
      
      formattedLog.append("[pre]");
    }
    
    for(int index = 0;index < consoleBuffer.length();++index) {
      
      if(consoleBuffer.charAt(index) == 0x1B) {
        
        tempIndex = index + 1;
        if(tempIndex < consoleBuffer.length() && consoleBuffer.charAt(tempIndex) == '[') {
          
          numberTextStringBuffer = new StringBuffer();
          ++tempIndex;
          
          while(tempIndex < consoleBuffer.length() && Character.isDigit(consoleBuffer.charAt(tempIndex))) {
            
            numberTextStringBuffer.append(consoleBuffer.charAt(tempIndex));
            
            ++tempIndex;
          }
          
          if(numberTextStringBuffer.length() > 0 && tempIndex < consoleBuffer.length() && consoleBuffer.charAt(tempIndex) == 'm') {
            
            int colorCode = Integer.valueOf(numberTextStringBuffer.toString());
            index = tempIndex;
            
            TelnetColor newColor = TelnetColor.getTelnetColorByColorCode(colorCode);
            if(newColor != null) {
              
              if(!currentColor.equals(TelnetColor.normal)) {
              
                if(htmlMode)
                  formattedLog.append("</span>");
                else if(forumMode)
                  formattedLog.append("[/color]");
              }
              

              if(isBolded) {
                
                if(htmlMode)
                  formattedLog.append("</span>");
                else if(forumMode)
                  formattedLog.append("[/b]");
                isBolded = false;
              }
              
              if(!newColor.equals(TelnetColor.normal) && !newColor.equals(TelnetColor.bold)) {
                
                if(htmlMode)
                  formattedLog.append("<span style='color: #" + newColor.getHexCode() + ";'>");
                else if(forumMode)
                  formattedLog.append("[color=#" + newColor.getHexCode() + "]");
                
                if(isBolded) {
                  
                  if(htmlMode)
                    formattedLog.append("<span style='font-weight:bold;'>");
                  else if(forumMode)
                    formattedLog.append("[b]");
                }
              }
              else if(newColor.equals(TelnetColor.bold)) {
                
                if(htmlMode)
                  formattedLog.append("<span style='font-weight:bold;'>");
                else if(forumMode)
                  formattedLog.append("[b]");
                isBolded = true;
              }
              
              if(newColor != null && !newColor.equals(TelnetColor.bold))
                currentColor = newColor;
            }
          }
        }
      }
      else {
        
        formattedLog.append(consoleBuffer.charAt(index));
      }
    }
    
    if(forumMode) {
      formattedLog.append("[/pre]");
    }

    return formattedLog.toString();
  }
}
