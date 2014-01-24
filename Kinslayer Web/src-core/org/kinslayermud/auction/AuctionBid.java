package org.kinslayermud.auction;

import org.kinslayermud.misc.DataObjectWithIntId;

public class AuctionBid extends DataObjectWithIntId {

  protected int auctionId;
  protected String objectId;
  protected int bidderId;
  protected int auctionItemId;
  protected long bidAmount;
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
  public int getBidderId() {
    return bidderId;
  }
  public void setBidderId(int bidderId) {
    this.bidderId = bidderId;
  }
  public int getAuctionItemId() {
    return auctionItemId;
  }
  public void setAuctionItemId(int auctionItemId) {
    this.auctionItemId = auctionItemId;
  }
  public long getBidAmount() {
    return bidAmount;
  }
  public void setBidAmount(long bidAmount) {
    this.bidAmount = bidAmount;
  }
  public long getTimestamp() {
    return timestamp;
  }
  public void setTimestamp(long timestamp) {
    this.timestamp = timestamp;
  }
}
