package org.kinslayermud.web.playerportal.signedin;

import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashSet;
import java.util.List;
import java.util.Map;

import org.kinslayermud.character.User;
import org.kinslayermud.comm.Comm;
import org.kinslayermud.mob.MobPrototype;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.StringUtil;
import org.kinslayermud.util.WebSupport;

public class ViewTellHistoryAction extends ValidateSignInAction {

  protected final String SUCCESS_FORWARD = "Success";
  protected final String FAILURE_FORWARD = "Failure";

  protected final String PAGE_NUMBER_PARAMETER = "PageNumber";
  protected final int FETCH_SIZE = 50;
  
  public String execute(WebSupport webSupport, User user) throws Exception {
    
    int pageNumber = 1;
    String pageNumberParameter = StringUtil.removeNull(request.getParameter(PAGE_NUMBER_PARAMETER));
    
    if(MiscUtil.isValidIntString(pageNumberParameter)) {
      
      int pageNumberParameterInt = Integer.valueOf(pageNumberParameter);
      
      if(pageNumberParameterInt > 1) {
        
        pageNumber = pageNumberParameterInt;
      }
    }
    int offset = ((pageNumber-1) * FETCH_SIZE);
    boolean hasNextPage = false;
    
    List<Comm> tellHistory = webSupport.getTellHistory(user.getUserId(), offset, FETCH_SIZE + 1);
    
    Collections.sort(tellHistory, new Comparator<Comm>() {

      public int compare(Comm arg0, Comm arg1) {
        
        return arg0.getTimestamp().compareTo(arg1.getTimestamp());
      }
    });
    
    Collection<Integer> userIdCollection = new HashSet<Integer>();
    Collection<Integer> mobPrototypeIdCollection = new HashSet<Integer>();
    for(Comm comm : tellHistory) {
      
      if(comm.getSenderType() == 'C') {
        userIdCollection.add(comm.getSenderUserId());
      }
      else if(comm.getSenderType() == 'M') {
        mobPrototypeIdCollection.add(comm.getSenderUserId());
      }
      
      if(comm.getRecipientType() == 'C') {
        userIdCollection.add(comm.getRecipientUserId());
      }
      else if(comm.getRecipientType() == 'M') {
        mobPrototypeIdCollection.add(comm.getRecipientUserId());
      }
    }
    
    if(tellHistory.size() > FETCH_SIZE) {
      tellHistory.remove(tellHistory.size() - 1);
      hasNextPage = true;
    }
    
    Map<Integer, MobPrototype> mobPrototypeMap = webSupport.getMobPrototypeMap(mobPrototypeIdCollection);
    Map<Integer, User> userMap = webSupport.getUserMap(userIdCollection);
    
    request.setAttribute("User", user);
    request.setAttribute("TellHistory", tellHistory);
    request.setAttribute("UserMap", userMap);
    request.setAttribute("MobPrototypeMap", mobPrototypeMap);
    request.setAttribute("PageNumber", pageNumber);
    request.setAttribute("HasNextPage", hasNextPage);
    
    return SUCCESS_FORWARD;
  }
}
