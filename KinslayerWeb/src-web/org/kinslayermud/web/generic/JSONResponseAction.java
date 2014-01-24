package org.kinslayermud.web.generic;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts2.interceptor.ServletRequestAware;
import org.apache.struts2.interceptor.ServletResponseAware;

import com.google.gson.Gson;
import com.opensymphony.xwork2.ActionSupport;

public class JSONResponseAction extends ActionSupport implements ServletRequestAware, ServletResponseAware {

  protected HttpServletRequest request;
  protected HttpServletResponse response;
  
  public String execute() throws Exception {

    Object jsonObject = request.getAttribute("JSONObject");
    
    Gson gson = new Gson();
    String json = gson.toJson(jsonObject);
    response.getWriter().write(json);
    
    return null;
  }

  public void setServletRequest(HttpServletRequest request) {

    this.request = request;
  }

  public void setServletResponse(HttpServletResponse response) {

    this.response = response;
  }
}
