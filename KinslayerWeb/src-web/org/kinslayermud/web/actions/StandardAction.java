package org.kinslayermud.web.actions;

import javax.servlet.ServletContext;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts2.interceptor.ServletRequestAware;
import org.apache.struts2.interceptor.ServletResponseAware;
import org.apache.struts2.util.ServletContextAware;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.util.HttpUtil;

import com.opensymphony.xwork2.ActionSupport;

public abstract class StandardAction extends ActionSupport implements ServletRequestAware, ServletResponseAware, ServletContextAware {

  protected HttpServletRequest request;
  protected HttpServletResponse response;
  protected ServletContext servletContext;
  protected WebSupport webSupport;
  
  protected final String SUCCESS_FORWARD = "Success";
  protected final String FAILURE_FORWARD = "Failure";
  
  public StandardAction() {

  }
  
  public abstract String execute(WebSupport webSupport) throws Exception;
  
  public String execute() throws Exception {
  
    this.webSupport = (WebSupport)servletContext.getAttribute("WebSupport");
    request.setAttribute("WebSupport", webSupport);
    
    Cookie sessionUserNameCookie = HttpUtil.getCookieByName(request, "SESSIONUSERNAME");
    if(sessionUserNameCookie != null) {
      
      request.setAttribute("SessionUserName", sessionUserNameCookie.getValue());
    }
    
    Cookie sessionUserIdCookie = HttpUtil.getCookieByName(request, "SESSIONUSERID");
    if(sessionUserIdCookie != null && MiscUtil.isValidIntString(sessionUserIdCookie.getValue())) {
      
      request.setAttribute("SessionUserId", Integer.valueOf(sessionUserIdCookie.getValue()));
    }
    
    Cookie sessionIdCookie = HttpUtil.getCookieByName(request, "SESSIONID");
    if(sessionIdCookie != null) {
      
      request.setAttribute("SessionId", sessionIdCookie.getValue());
    }
    
    try {
      return execute(webSupport);
    }
    catch(Exception exception) {
      
      exception.printStackTrace();
      return null;//TODO: Forward to error page.
    }
  }
  
  public void setServletRequest(HttpServletRequest request) {

    this.request = request;
  }

  public void setServletResponse(HttpServletResponse response) {

    this.response = response;
  }
  
  public void setServletContext(ServletContext servletContext) {
    
    this.servletContext = servletContext;
  }
}
