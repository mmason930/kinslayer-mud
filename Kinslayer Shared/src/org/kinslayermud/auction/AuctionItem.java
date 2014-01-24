package org.kinslayermud.auction;

import org.kinslayermud.misc.DataObjectWithIntId;

public class AuctionItem extends DataObjectWithIntId {

  protected int auctionId;
  protected String objectId;
  protected int ownerId;
  protected long endTime;
  protected long startingPrice;
  protected long buyoutPrice;
  protected boolean active;
  protected boolean retrieved;
  protected long timestamp;
  
  public int getAuctionId() {
    return auctionId;
  }
  
  public void setAuctionId(int auctionId) {
    this.auctionId = auctionId;
  }
  
  public String getObjectId() {
    return objectId;
  }
  
  public void setObjectId(String objectId) {
    this.objectId = objectId;
  }
  
  public int getOwnerId() {
    return ownerId;
  }
  
  public void setOwnerId(int ownerId) {
    this.ownerId = ownerId;
  }
  
  public long getEndTime() {
    return endTime;
  }
  
  public void setEndTime(long endTime) {
    this.endTime = endTime;
  }
  
  public long getStartingPrice() {
    return startingPrice;
  }
  
  public void setStartingPrice(long startingPrice) {
    this.startingPrice = startingPrice;
  }
  
  public long getBuyoutPrice() {
    return buyoutPrice;
  }
  
  public void setBuyoutPrice(long buyoutPrice) {
    this.buyoutPrice = buyoutPrice;
  }
  
  public boolean getActive() {
    return active;
  }
  
  public void setActive(boolean active) {
    this.active = active;
  }
  
  public boolean getRetrieved() {
    return retrieved;
  }
  
  public void setRetrieved(boolean retrieved) {
    this.retrieved = retrieved;
  }
  
  public long getTimestamp() {
    return timestamp;
  }
  
  public void setTimestamp(long timestamp) {
    this.timestamp = timestamp;
  }  
}
