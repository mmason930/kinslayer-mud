package org.kinslayermud.web.servletcontext;

import org.kinslayermud.util.WebSupport;

public class KinslayerServiceThread extends Thread implements Runnable {

  protected WebSupport webSupport;
  protected volatile boolean running;
  
  protected final long HOME_RESOURCES_LOAD_MS = 15000;
  protected long timeOfLastHomeResourcesLoad = System.currentTimeMillis();
  
  public KinslayerServiceThread(WebSupport webSupport) {
    
    this.webSupport = webSupport;
    this.running = true;
  }
  
  public void run() {
    
    while(running) {
      
      long currentTime = System.currentTimeMillis();
      if(currentTime >= timeOfLastHomeResourcesLoad + HOME_RESOURCES_LOAD_MS) {
        
        webSupport.loadHomeResources();
        timeOfLastHomeResourcesLoad = System.currentTimeMillis();
      }
      
      try {
        Thread.sleep(1000);
      }
      catch (InterruptedException e) {
        running = false;
      }
    }
  }
  
  public void kill() {
    
    this.running = false;
  }
  
}
