package org.kinslayermud.user;

import java.util.Date;

import org.kinslayermud.misc.DataObjectWithIntId;

public class User extends DataObjectWithIntId {

  protected String emailAddress;
  protected String password;
  protected Date createdDatetime;
  
  public User() {
    
    id = NEW;
  }
  
  public String getEmailAddress() {
    
    return emailAddress;
  }
  
  public void setEmailAddress(String emailAddress) {
    
    this.emailAddress = emailAddress;
  }
  
  public String getPassword() {
    
    return password;
  }
  
  public void setPassword(String password) {
    
    this.password = password;
  }
  
  public Date getCreatedDatetime() {
    
    return createdDatetime;
  }
  
  public void setCreatedDatetime(Date createdDatetime) {
    
    this.createdDatetime = createdDatetime;
  }
}
