package org.kinslayermud.web.help;

import java.util.LinkedList;
import java.util.List;

import org.kinslayermud.help.HelpFile;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;

public class ViewHelpFileAction extends StandardAction {

  protected static String SUCCESS_FORWARD = "Success";
  protected static String FAILURE_FORWARD = "Failure";
  
  protected static String HELP_FILE_PATH_PARAMETER = "HelpFilePath";
  
  public static class ViewHelpFileData {
    
    public HelpFile helpFile;
    public String h1;
    public String currentCategoryText;
    public String description;
    public List<HelpFile> helpFileChain = new LinkedList<HelpFile>();
    public List<HelpFile> helpFileChainWithoutCurrentHelpFile = new LinkedList<HelpFile>();
    public List<HelpFile> childrenHelpFiles = new LinkedList<HelpFile>();
  }
  
  public String execute(WebSupport webSupport) throws Exception {

    List<HelpFile> helpFileChain = new LinkedList<HelpFile>();
    HelpFile helpFile = null;
    String helpFilePath = request.getParameter(HELP_FILE_PATH_PARAMETER);
    
    System.out.println("HELP FILE PATH: " + helpFilePath);
 
    if(helpFilePath != null) {
      String[] modRewrittenNameChain = helpFilePath.split("/");
    
      for(String modRewrittenName : modRewrittenNameChain) {
        
        System.out.println("MOD REWRITTEN NAME: " + modRewrittenName);
        helpFile = webSupport.getHelpFileByModRewrittenName(modRewrittenName, helpFile == null ? null : helpFile.getId());
        
        if(helpFile == null) {
          
          return FAILURE_FORWARD;
        }
        
        helpFileChain.add(helpFile);
      }

      if(helpFile == null) {
        
        return FAILURE_FORWARD;
      }
    }

    
    ViewHelpFileData viewHelpFileData = new ViewHelpFileData();
    
    if(helpFile != null) {
      viewHelpFileData.helpFile = helpFile;
      viewHelpFileData.h1 = helpFile.getName();
      viewHelpFileData.currentCategoryText = helpFile.getName();
      viewHelpFileData.helpFileChain = helpFileChain;
      viewHelpFileData.helpFileChainWithoutCurrentHelpFile.addAll(viewHelpFileData.helpFileChain);
      viewHelpFileData.helpFileChainWithoutCurrentHelpFile.remove(viewHelpFileData.helpFileChainWithoutCurrentHelpFile.size() - 1);
      viewHelpFileData.description = formatDescription(helpFile.getPageContent());
      viewHelpFileData.childrenHelpFiles = webSupport.getHelpFilesByParentId(helpFile.getId());
    }
    else {
      viewHelpFileData.h1 = "Help Files";
      viewHelpFileData.currentCategoryText = "Help";
      viewHelpFileData.description =
            "Welcome to the Kinslayer MUD help files. To the left, you will see the various help files that you "
          + "can browse.";
      viewHelpFileData.childrenHelpFiles = webSupport.getHelpFilesByParentId(null);
    }
    
    request.setAttribute("ViewHelpFileData", viewHelpFileData);
    
    return SUCCESS_FORWARD;
  }
  
  public String formatDescription(String description) {
    
    return description.replace("\n", "<br/>");
  }
}
