<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%@ page import='org.kinslayermud.web.help.ViewHelpFileAction.*' %>
<%
HelpFile selectedHelpFile = (HelpFile)request.getAttribute("SelectedHelpFile");
List<HelpFile> helpFileChain = (List<HelpFile>)request.getAttribute("HelpFileChain");
List<HelpFile> currentHelpFileChain = new LinkedList<HelpFile>();
%>
<a href="./help">Help</a>
<%
for(HelpFile helpFile : helpFileChain) {
  currentHelpFileChain.add(helpFile);
  String url = WebSiteUrlUtil.getHelpFileUrl(webSupport.getInstanceDomain(), currentHelpFileChain);
  boolean isCurrentHelpFile = (selectedHelpFile.getId() == helpFile.getId());
  
  System.out.println("Is Current Help File: " + isCurrentHelpFile);
  System.out.println("Selected Help File: " + selectedHelpFile.getId());
  System.out.println("Help File: " + helpFile.getId());
%>
 &gt; 
<%
  if(!isCurrentHelpFile) {
%>
<a href="<%=url%>"><%=helpFile.getName() %></a>
<%
  }
  else {
%>
<%=helpFile.getName() %>
<%
  }
}
%>