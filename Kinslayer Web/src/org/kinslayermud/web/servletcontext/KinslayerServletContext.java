package org.kinslayermud.web.servletcontext;

import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;

import org.kinslayermud.misc.Provider;
import org.kinslayermud.util.WebSupport;

public class KinslayerServletContext implements ServletContextListener {
  
  public void contextDestroyed(ServletContextEvent servletContextEvent) {
    
  }

  public void contextInitialized(ServletContextEvent servletContextEvent) {
    
    try {
      Provider provider = new Provider();
      provider.loadConfiguration(servletContextEvent.getServletContext().getInitParameter("ConfigurationPath"));
      WebSupport webSupport = new WebSupport(provider);
    
      servletContextEvent.getServletContext().setAttribute("WebSupport", webSupport);
    }
    catch(Exception exception) {
      
      System.out.println("FATAL ERROR: Could not intialize KinslayerServletContext");
      exception.printStackTrace();
    }
  }
}
