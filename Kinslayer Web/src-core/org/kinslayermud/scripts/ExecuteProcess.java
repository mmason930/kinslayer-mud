package org.kinslayermud.scripts;

import org.kinslayermud.misc.Provider;
import org.kinslayermud.util.MiscUtil;

public class ExecuteProcess {

  public static void main(String[] args) throws Exception {
    
    if(args.length < 2) {
      
      throw new Exception("Too few arguments provided.");
    }
    
    String configurationFilePath = args[ 0 ];
    Provider provider = new Provider();
    provider.loadConfiguration(configurationFilePath);
    
    String processClassName = args[ 1 ];
    Class processClass = Class.forName(processClassName);
    
    Object processObject = processClass.newInstance();
    if(! (processObject instanceof KinslayerProcess) ) {
      
      throw new Exception("Class `" + processClassName + "` is not an instance of AffiliateProcess.");
    }
    
    KinslayerProcess storeProcess = (KinslayerProcess)processObject;
    
    MiscUtil.log("Spawning Process `" + processClassName + "`...");
    storeProcess.process(provider);
    MiscUtil.log("Process Closing Down...");
  }
}
