package org.kinslayermud.web.servletcontext;

import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;

import org.kinslayermud.misc.Provider;
import org.kinslayermud.util.WebSupport;

public class KinslayerServletContext implements ServletContextListener {
  
  protected KinslayerServiceThread kinslayerServiceThread;
  
  public void contextDestroyed(ServletContextEvent servletContextEvent) {
    
    kinslayerServiceThread.kill();
    try {
      kinslayerServiceThread.join();
    }
    catch (InterruptedException e) {
      e.printStackTrace();
    }
  }

  public void contextInitialized(ServletContextEvent servletContextEvent) {
    
    try {
      Provider provider = new Provider();
      provider.loadConfiguration(servletContextEvent.getServletContext().getInitParameter("ConfigurationPath"));
      WebSupport webSupport = new WebSupport(provider);
    
      servletContextEvent.getServletContext().setAttribute("WebSupport", webSupport);
      
      KinslayerServiceThread kinslayerServiceThread = new KinslayerServiceThread(webSupport);
      kinslayerServiceThread.setName("Kinslayer Service Thread");
      kinslayerServiceThread.start();
    }
    catch(Exception exception) {
      
      System.out.println("FATAL ERROR: Could not intialize KinslayerServletContext");
      exception.printStackTrace();
    }
  }
}
