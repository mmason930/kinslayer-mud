package org.kinslayermud.comm;

import java.util.Date;

import org.kinslayermud.misc.DataObjectWithIntId;

public class Comm extends DataObjectWithIntId {

  protected String type;
  protected String message;
  protected char senderType;
  protected int senderUserId;
  protected char recipientType;
  protected int recipientUserId;
  protected int roomVnum;
  protected int invisLevel;
  protected Date timestamp;
  public String getType() {
    return type;
  }
  public void setType(String type) {
    this.type = type;
  }
  public String getMessage() {
    return message;
  }
  public void setMessage(String message) {
    this.message = message;
  }
  public char getSenderType() {
    return senderType;
  }
  public void setSenderType(char senderType) {
    this.senderType = senderType;
  }
  public int getSenderUserId() {
    return senderUserId;
  }
  public void setSenderUserId(int senderUserId) {
    this.senderUserId = senderUserId;
  }
  public char getRecipientType() {
    return recipientType;
  }
  public void setRecipientType(char recipientType) {
    this.recipientType = recipientType;
  }
  public int getRecipientUserId() {
    return recipientUserId;
  }
  public void setRecipientUserId(int recipientUserId) {
    this.recipientUserId = recipientUserId;
  }
  public int getRoomVnum() {
    return roomVnum;
  }
  public void setRoomVnum(int roomVnum) {
    this.roomVnum = roomVnum;
  }
  public int getInvisLevel() {
    return invisLevel;
  }
  public void setInvisLevel(int invisLevel) {
    this.invisLevel = invisLevel;
  }
  public Date getTimestamp() {
    return timestamp;
  }
  public void setTimestamp(Date timestamp) {
    this.timestamp = timestamp;
  }
}
