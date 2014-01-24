package org.kinslayermud.userlog;

import java.util.Date;

import org.kinslayermud.misc.DataObjectWithIntId;

public class UserLogRecord  extends DataObjectWithIntId {

  public UserLogRecord() {
    
    id = NEW;
  }
  
  protected int userId;
  protected Date recordDate;
  
  public int getUserId() {
    
    return userId;
  }
  
  public void setUserId(int userId) {
    
    this.userId = userId;
  }
  
  public Date getRecordDate() {
    
    return recordDate;
  }
  
  public void setRecordDate(Date recordDate) {
    
    this.recordDate = recordDate;
  }
}
