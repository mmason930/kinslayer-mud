package org.kinslayermud.web.actions;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts2.interceptor.ServletRequestAware;
import org.apache.struts2.interceptor.ServletResponseAware;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.util.WebSupportImp;

import com.opensymphony.xwork2.ActionSupport;

public abstract class StandardAction extends ActionSupport implements ServletRequestAware, ServletResponseAware {

  protected HttpServletRequest request;
  protected HttpServletResponse response;
  protected WebSupport webSupport;
  
  protected static final String CART_ID_COOKIE_NAME = "CartId";
  protected static final String CART_RANDOM_ID_COOKIE_NAME = "CartRandomId";
  
  public StandardAction() {

    webSupport = new WebSupportImp();
  }
  
  public abstract String execute(WebSupport webSupport) throws Exception;
  
  public String execute() throws Exception {
  
    return execute(webSupport);
  }
  
  public void setServletRequest(HttpServletRequest request) {

    this.request = request;
  }

  public void setServletResponse(HttpServletResponse response) {

    this.response = response;
  }
}
