package org.kinslayermud.web.playerportal.signedin;

import org.kinslayermud.character.User;
import org.kinslayermud.userlog.UserLog;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.StringUtil;
import org.kinslayermud.util.WebSupport;

public class ViewUserLogAction extends ValidateSignInAction {

  protected String SUCCESS_FORWARD = "Success";
  protected String FAILURE_FORWARD = "Failure";
  
  protected String USERLOGID_PARAMETER = "UserLogId";
  
  public String execute(WebSupport webSupport, User user) throws Exception {
    
    String userLogIdString = request.getParameter(USERLOGID_PARAMETER);
    
    if(userLogIdString != null && MiscUtil.isValidIntString(userLogIdString)) {
      
      UserLog userLog = webSupport.getUserLog(Integer.valueOf(userLogIdString));
      
      if(userLog.getUserId() == user.getUserId()) {
        
        userLog.setConsoleOutput(formatUserLog(userLog.getConsoleOutput()));
        request.setAttribute("UserLog", userLog);
        return SUCCESS_FORWARD;
      }
    }
    
    return FAILURE_FORWARD;
  }
  
  public String formatUserLog(String consoleBuffer) {
    
    StringBuffer formattedLog = new StringBuffer();
    StringBuffer numberTextStringBuffer = new StringBuffer();
    int tempIndex;
    
    for(int index = 0;index < consoleBuffer.length();++index) {
      
      if(consoleBuffer.charAt(index) == 0x1B) {
        
        System.out.println("ESCAPE CHARACTER FOUND.");
        
        tempIndex = index + 1;
        if(index < consoleBuffer.length() && consoleBuffer.charAt(index) == '[') {
          
          System.out.println("BRACKET FOUND.");
          numberTextStringBuffer = new StringBuffer();
          ++tempIndex;
          
          while(tempIndex < consoleBuffer.length() && Character.isDigit(consoleBuffer.charAt(tempIndex))) {
            
            numberTextStringBuffer.append(consoleBuffer.charAt(tempIndex));
            
            ++tempIndex;
          }
          
          System.out.println("TOTAL NUMBER TEXT BUFFER: " + numberTextStringBuffer.toString());
          
          if(numberTextStringBuffer.length() > 0 && tempIndex < consoleBuffer.length() && consoleBuffer.charAt(tempIndex) == 'm') {
            
            int colorCode = Integer.valueOf(numberTextStringBuffer.toString());
            index = tempIndex;
          }
        }
      }
      else {
        
        formattedLog.append(consoleBuffer.charAt(index));
      }
    }

    return StringUtil.ConvertToHTML(formattedLog.toString());
  }
  
}
