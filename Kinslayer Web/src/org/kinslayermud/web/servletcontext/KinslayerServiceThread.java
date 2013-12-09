package org.kinslayermud.web.servletcontext;

import org.kinslayermud.util.WebSupport;

public class KinslayerServiceThread extends Thread implements Runnable {

  protected WebSupport webSupport;
  protected volatile boolean running;
  
  public KinslayerServiceThread(WebSupport webSupport) {
    
    this.webSupport = webSupport;
    this.running = true;
  }
  
  public void run() {
    
    while(running) {
      
      Thread.yield();
    }
  }
  
  public void kill() {
    
    this.running = false;
  }
  
}
