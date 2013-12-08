package org.kinslayermud.web.servletcontext;

import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;

public class KinslayerServletContext implements ServletContextListener {

  public void contextDestroyed(ServletContextEvent arg0) {
    
  }

  public void contextInitialized(ServletContextEvent arg0) {
    
    System.out.println("Servlet Context Created!!1!");
  }
}
