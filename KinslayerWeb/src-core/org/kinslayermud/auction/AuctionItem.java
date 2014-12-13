package org.kinslayermud.auction;

import java.sql.ResultSet;
import java.sql.SQLException;

import org.kinslayermud.dbutils.DBObject;
import org.kinslayermud.misc.DataObjectWithIntId;
import org.kinslayermud.util.QueryUtil;

public class AuctionItem extends DataObjectWithIntId implements DBObject {

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
  
  public void retrieveFromResultSet(ResultSet resultSet) throws SQLException {
    
    setId(resultSet.getInt("auctionItem.id"));
    setAuctionId(resultSet.getInt("auctionItem.auction_id"));
    setObjectId(resultSet.getString("auctionItem.object_id"));
    setOwnerId(resultSet.getInt("auctionItem.owner_id"));
    setEndTime(resultSet.getLong("auctionItem.end_time"));
    setStartingPrice(resultSet.getLong("auctionItem.starting_price"));
    setBuyoutPrice(resultSet.getLong("auctionItem.buyout_price"));
    setActive(QueryUtil.getIntBoolean(resultSet, "auctionItem.active"));
    setRetrieved(QueryUtil.getIntBoolean(resultSet, "auctionItem.retrieved"));
    setTimestamp(resultSet.getLong("auctionItem.timestamp"));
  }
}
