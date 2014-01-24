package org.kinslayermud.character;

import java.util.Date;


public class User {

  protected int userId;
  protected String userName;
  protected int level;
  protected Date lastLogout;
  protected Date lastLogon;
  protected UserClass userClass;
  protected UserRace userRace;
  
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
  
  public int getLevel() {
    
    return level;
  }
  
  public void setLevel(int level) {
    
    this.level = level;
  }

  public Date getLastLogout() {
    return lastLogout;
  }

  public void setLastLogout(Date lastLogout) {
    this.lastLogout = lastLogout;
  }

  public Date getLastLogon() {
    return lastLogon;
  }

  public void setLastLogon(Date lastLogon) {
    this.lastLogon = lastLogon;
  }

  public UserClass getUserClass() {
    return userClass;
  }

  public void setUserClass(UserClass userClass) {
    this.userClass = userClass;
  }

  public UserRace getUserRace() {
    return userRace;
  }

  public void setUserRace(UserRace userRace) {
    this.userRace = userRace;
  }
}
