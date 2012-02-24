import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.regex.Matcher;
import java.util.regex.Pattern;


public class BuildExternalScripts {

  public static String loadClassPathBuffer() throws IOException {
    
    StringBuffer buffer = new StringBuffer();
    
    BufferedReader reader = new BufferedReader( new FileReader(wrapTargetPath(".classpath")));
    
    while( reader.ready() ) {
      
      String line = reader.readLine();
      
      buffer.append(line);
      
    }
    
    return buffer.toString();
  }
  public static String wrapTargetPath(String dest) {
    
    return "./ExternalScripts/" + dest + "";
  }
  
  public static String getClassPathArgument( String classPathBuffer, String prefix ) {
    
    String commandArguments = "";
    boolean firstClassPathMatch = true;
    
    Pattern classPathPattern = Pattern.compile("<classpathentry kind=\"lib\" path=\"(.*?)\"/>");
    Matcher classPathMatcher = classPathPattern.matcher(classPathBuffer);
    
    while( classPathMatcher.find() ) {
      
      if( firstClassPathMatch == true ) {
        
        commandArguments += " -cp .";
      }
      
      firstClassPathMatch = false;
      
      String cp = classPathMatcher.group(1); 
      
      commandArguments += ":" + prefix +
      (prefix.equals("") ? wrapTargetPath(cp) : cp);
    }
    
    return commandArguments;
  }
  
  public static String getSourceArgument(String classPathBuffer) {
    
    Pattern sourcePattern = Pattern.compile("<classpathentry kind=\"src\" path=\"(.*?)\"/>");
    Matcher sourceMatcher = sourcePattern.matcher(classPathBuffer);
    
    String commandArguments = "";
    
    while( sourceMatcher.find() ) {
      
      commandArguments += " " + wrapTargetPath(sourceMatcher.group(1));
      
    }
    
    return commandArguments;
  }
  
  public static void compile() throws IOException, InterruptedException {
    
    Runtime rt=Runtime.getRuntime();
    String classPathBuffer = loadClassPathBuffer();
    String commandArguments = "javac -1.5";
    Process process = null;
    
    commandArguments += getClassPathArgument(classPathBuffer, "");
    commandArguments += getSourceArgument(classPathBuffer);
    
    //System.out.println("Command: " + commandArguments);
    
    process = rt.exec(commandArguments);

    process.waitFor();
    
    System.out.println( getInputFromInputStream(process.getErrorStream()));

    System.out.println( getInputFromInputStream(process.getInputStream()));
    
  }
  
  public static void run(String classPath, String arg1) throws IOException, InterruptedException {
    
    Runtime rt=Runtime.getRuntime();
    String classPathBuffer = loadClassPathBuffer();
    String commandArguments = "java";
    Process process = null;
    
    commandArguments += getClassPathArgument(classPathBuffer, "../");
    
    commandArguments += " " + classPath;

    if( arg1 != null ) {
      
      commandArguments += " " + arg1;
    }
    
    process = rt.exec(commandArguments, null, new File("./ExternalScripts/src"));
    
    //System.out.println("Command: " + commandArguments);
    
    process.waitFor();
    
    System.out.println( getInputFromInputStream(process.getErrorStream()));
    
    System.out.println( getInputFromInputStream(process.getInputStream()));
  }
  
  public static String getInputFromInputStream(InputStream inputStream) throws IOException {
    
    StringBuffer buffer = new StringBuffer();
    
    InputStreamReader r = new InputStreamReader(inputStream);
    
    while( r.ready() ) {
      
      char[] buf = new char[1024];
      int ret = r.read(buf, 0, 1024);
      
      buffer.append(buf, 0, ret);
    }
    
    return buffer.toString();
  }

  public static void main(String []args) throws Exception {
    
    /***
    try {
      InputStream i = new FileInputStream(new File("test.txt"));
      
      System.out.println("File contents:\n" + getInputFromInputStream(i));
      
      System.exit(1);
    }
    catch( Exception t ) {
      
      t.printStackTrace();
      return;
    }
    ***/
    
    if( args.length > 0 ) {
      
      if( args[0].equals("build") ) {
        
        compile();
      }
      else if( args[0].equals("run") ) {
        
        if( args.length > 1 ) {
          
          String classPath = args[1];
          
          run(classPath, args.length > 2 ? args[2] : null);
        }
        else {
          
          System.out.println("Attempting to run, but no class path specified.");
          return;
        }
      }
      else {
        
        System.out.println("Invalid argument specified : " + args[0]);
        return;
      }
    }
    else {
      
      System.out.println("No command specified.");
      return;
    }
        
  }
  
}
