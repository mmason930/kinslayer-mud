package org.kinslayermud.character;


public class User {

  protected int userId;
  protected String userName;
  
  public User() {
    
  }

  public int getUserId() {
    
    return userId;
  }
  
  public void setUserId(int userId) {
    
    this.userId = userId;
  }
  
  public String getUserName() {
    
    return userName;
  }
  
  public void setUserName(String userName) {
    
    this.userName = userName;
  }
}
