package org.kinslayermud.web.playerportal.signedin;

import org.kinslayermud.character.User;
import org.kinslayermud.exception.DataInterfaceException;
import org.kinslayermud.telnet.TelnetColor;
import org.kinslayermud.userlog.UserLog;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.StringUtil;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.util.WebSupportImp;

public class ViewUserLogAction extends ValidateSignInAction {

  protected String SUCCESS_FORWARD = "Success";
  protected String FAILURE_FORWARD = "Failure";
  
  protected String USERLOGID_PARAMETER = "UserLogId";
  
  public String execute(WebSupport webSupport, User user) throws Exception {
    
    String userLogIdString = request.getParameter(USERLOGID_PARAMETER);
    
    if(userLogIdString != null && MiscUtil.isValidIntString(userLogIdString)) {
      
      UserLog userLog = webSupport.getUserLog(Integer.valueOf(userLogIdString));
      
      if(userLog.getUserId() == user.getUserId()) {
        
        userLog.setConsoleOutput(formatUserLog(StringUtil.ConvertToHTML(userLog.getConsoleOutput())));
        request.setAttribute("UserLog", userLog);
        return SUCCESS_FORWARD;
      }
    }
    
    return FAILURE_FORWARD;
  }
  
  public static String formatUserLog(String consoleBuffer) {
    
    StringBuffer formattedLog = new StringBuffer();
    StringBuffer numberTextStringBuffer = new StringBuffer();
    int tempIndex;
    TelnetColor currentColor = TelnetColor.normal;
    boolean isBolded = false;
    
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
System.out.println("NEW COLOR: " + (newColor == null ? "<NULL>" : newColor.getStandardName()));
            if(newColor != null) {
              
              if(!currentColor.equals(TelnetColor.normal)) {
              
                formattedLog.append("</span>");
                
                if(isBolded && newColor.equals(TelnetColor.normal)) {
                  formattedLog.append("</span>");
                  isBolded = false;
                }
              }
              
              if(!newColor.equals(TelnetColor.normal) && !newColor.equals(TelnetColor.bold)) {
                
                formattedLog.append("<span style='color: #" + newColor.getHexCode() + ";'>");
                if(isBolded) {
                  formattedLog.append("<span style='font-weight:bold;'>");
                }
                currentColor = newColor;
              }
              else if(newColor.equals(TelnetColor.bold)) {
                formattedLog.append("<span style='font-weight:bold;'>");
                isBolded = true;
              }
            }
            System.out.println("COLOR CODE: " + colorCode);
          }
        }
      }
      else {
        
        formattedLog.append(consoleBuffer.charAt(index));
      }
    }

    return formattedLog.toString();
  }
  
  
  public static void main(String[] args) throws DataInterfaceException {
    
    WebSupport webSupport = new WebSupportImp();
    
    UserLog userLog = webSupport.getUserLog(7);
    formatUserLog(userLog.getConsoleOutput());
  }
}
