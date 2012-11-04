package org.kinslayermud.web.home;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts2.interceptor.ServletRequestAware;
import org.apache.struts2.interceptor.ServletResponseAware;

import com.opensymphony.xwork2.ActionSupport;

public class HomeAction extends ActionSupport implements ServletRequestAware, ServletResponseAware {


  private HttpServletRequest request;
  private HttpServletResponse response;

  private static String SUCCESS_FORWARD = "Success";
  public String execute() {

    System.out.println("HomeAction start!");
    
    return SUCCESS_FORWARD;
  }

  public void setServletRequest(HttpServletRequest request) {

    this.request = request;
  }

  public void setServletResponse(HttpServletResponse response) {

    this.response = response;
  }
}
