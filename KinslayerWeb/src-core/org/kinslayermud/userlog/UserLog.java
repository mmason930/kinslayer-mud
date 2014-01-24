package org.kinslayermud.userlog;

import java.util.Date;

import org.kinslayermud.misc.DataObjectWithIntId;

public class UserLog extends DataObjectWithIntId {

  protected int userId;
  protected String consoleOutput;
  protected Date recordDate;
  
  public UserLog() {
    
    id = NEW;
  }

  public int getUserId() {
    return userId;
  }

  public void setUserId(int userId) {
    this.userId = userId;
  }

  public String getConsoleOutput() {
    return consoleOutput;
  }

  public void setConsoleOutput(String consoleOutput) {
    this.consoleOutput = consoleOutput;
  }

  public Date getRecordDate() {
    return recordDate;
  }

  public void setRecordDate(Date recordDate) {
    this.recordDate = recordDate;
  }
  
  
}
