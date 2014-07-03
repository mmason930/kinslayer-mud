<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
User user = (User)request.getAttribute("User");
List<Comm> tellHistory = (List<Comm>)request.getAttribute("TellHistory");
Map<Integer, MobPrototype> mobPrototypeMap = (Map<Integer, MobPrototype>)request.getAttribute("MobPrototypeMap");
Map<Integer, User> userMap = (Map<Integer, User>)request.getAttribute("UserMap");
int pageNumber = (Integer)request.getAttribute("PageNumber");
boolean hasNextPage = (Boolean)request.getAttribute("HasNextPage");
%>
<div style="margin-top: 2%;"></div>
<div class="tellHistoryPagination">
<%
if(pageNumber > 1) {
%>
 <a href="<%=WebSiteUrlUtil.getPlayerPortalViewTellHistoryUrl(pageNumber - 1)%>">
<%
}
%>
 &lt;&lt;
<%
if(pageNumber > 1) {
%>
 </a>
<%
}
%>
 &nbsp;Page <%=pageNumber %>&nbsp;
<%
if(hasNextPage) {
%>
 <a href="<%=WebSiteUrlUtil.getPlayerPortalViewTellHistoryUrl(pageNumber + 1) %>">
<%
}
%>
 &gt;&gt;
<%
if(hasNextPage) {
%>
</a>
<%
}
%>
</div>
<%
for(Comm comm : tellHistory) {
  String timestamp = MiscUtil.formatDateMMMsDDcsYYYYsHHsMMsSS(comm.getTimestamp());
  String prefix, username = null, url = null;
  
  if(comm.getSenderType() == 'C' && comm.getSenderUserId() == user.getUserId()) {
    
    if(comm.getInvisLevel() > user.getLevel()) {
      username = "Someone";
    }
    else if(comm.getRecipientType() == 'M') {
      if(mobPrototypeMap.containsKey(comm.getRecipientUserId())) {
        username = mobPrototypeMap.get(comm.getRecipientUserId()).getShortDescription();
      }
      else {
        username = "&lt;Unknown MOB&gt;";
      }
    }
    else if(comm.getRecipientType() == 'C') {
      if(userMap.containsKey(comm.getRecipientUserId())) {
        User tellUser = userMap.get(comm.getRecipientUserId());
        url = WebSiteUrlUtil.getUserProfileUrl(webSupport.getInstanceDomain(), tellUser.getUserName());
        username = "<a href='" + url + "'>" + tellUser.getUserName() + "</a>";
      }
      else {
        username = "&lt;Deleted Player&gt;";
      }
    }
    prefix = "You tell " + username;
  }
  else {
    
    if(comm.getInvisLevel() > user.getLevel()) {
      username = "Someone";
    }
    else if(comm.getSenderType() == 'M') {
      if(mobPrototypeMap.containsKey(comm.getSenderUserId())) {
        username = mobPrototypeMap.get(comm.getSenderUserId()).getShortDescription();
      }
      else {
        username = "&lt;Unknown MOB&gt;";
      }
    }
    else if(comm.getSenderType() == 'C') {
      if(userMap.containsKey(comm.getSenderUserId())) {
        User tellUser = userMap.get(comm.getSenderUserId());
        url = WebSiteUrlUtil.getUserProfileUrl(webSupport.getInstanceDomain(), tellUser.getUserName());
        username = "<a href='" + url + "'>" + tellUser.getUserName() + "</a>";
      }
      else {
        username = "&lt;Deleted Player&gt;";
      }
    }
    prefix = username + " tells you";
  }
%>
[<%=timestamp%>] <%=prefix%>, '<%=comm.getMessage() %>'<br/>
<%
}
%>
