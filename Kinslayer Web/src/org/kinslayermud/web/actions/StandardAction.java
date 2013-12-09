package org.kinslayermud.web.actions;

import javax.servlet.ServletContext;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts2.interceptor.ServletRequestAware;
import org.apache.struts2.interceptor.ServletResponseAware;
import org.apache.struts2.util.ServletContextAware;
import org.kinslayermud.misc.Provider;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.util.HttpUtil;

import com.opensymphony.xwork2.ActionSupport;

public abstract class StandardAction extends ActionSupport implements ServletRequestAware, ServletResponseAware, ServletContextAware {

  protected HttpServletRequest request;
  protected HttpServletResponse response;
  protected ServletContext servletContext;
  protected WebSupport webSupport;
  
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
    if(sessionIdCookie != null && MiscUtil.isValidIntString(sessionIdCookie.getValue())) {
      
      request.setAttribute("SessionId", Integer.valueOf(sessionIdCookie.getValue()));
    }
    
    return execute(webSupport);
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
