package org.kinslayermud.auction;

import java.sql.ResultSet;
import java.sql.SQLException;

import org.kinslayermud.dbutils.DBObject;
import org.kinslayermud.misc.DataObjectWithIntId;

public class AuctionBid extends DataObjectWithIntId implements DBObject {

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
  
  public void retrieveFromResultSet(ResultSet resultSet) throws SQLException {
    
    setId(resultSet.getInt("auctionBid.id"));
    setAuctionId(resultSet.getInt("auctionBid.auction_id"));
    setObjectId(resultSet.getString("auctionBid.object_id"));
    setBidderId(resultSet.getInt("bidder_id"));
    setAuctionItemId(resultSet.getInt("auctionBid.ai_id"));
    setBidAmount(resultSet.getLong("auctionBid.bid_amount"));
    setTimestamp(resultSet.getLong("auctionBid.timestamp"));
  }
}
