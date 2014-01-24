package org.kinslayermud.auction;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.kinslayermud.util.QueryUtil;
import org.kinslayermud.util.SQLUtil;

public class AuctionUtil {

  public static AuctionItem getAuctionItem(ResultSet resultSet) throws SQLException {
    
    AuctionItem auctionItem = new AuctionItem();

    auctionItem.setId(resultSet.getInt("auctionItem.id"));
    auctionItem.setAuctionId(resultSet.getInt("auctionItem.auction_id"));
    auctionItem.setObjectId(resultSet.getString("auctionItem.object_id"));
    auctionItem.setOwnerId(resultSet.getInt("auctionItem.owner_id"));
    auctionItem.setEndTime(resultSet.getLong("auctionItem.end_time"));
    auctionItem.setStartingPrice(resultSet.getLong("auctionItem.starting_price"));
    auctionItem.setBuyoutPrice(resultSet.getLong("auctionItem.buyout_price"));
    auctionItem.setActive(QueryUtil.getIntBoolean(resultSet, "auctionItem.active"));
    auctionItem.setRetrieved(QueryUtil.getIntBoolean(resultSet, "auctionItem.retrieved"));
    auctionItem.setTimestamp(resultSet.getLong("auctionItem.timestamp"));
    
    return auctionItem;
  }
  
  public static List<AuctionItem> getActiveAuctionItems(Statement statement) throws SQLException {
    
    String sql = " SELECT *"
               + " FROM auctionItem"
               + " WHERE active = " + SQLUtil.encodeBooleanInt(true);
    
    ResultSet resultSet = statement.executeQuery(sql);
    List<AuctionItem> auctionItems = new ArrayList<AuctionItem>();
    
    while(resultSet.next()) {
      
      auctionItems.add(getAuctionItem(resultSet));
    }
    
    return auctionItems;
  }
  
  public static AuctionBid getAuctionBid(ResultSet resultSet) throws SQLException {
    
    AuctionBid auctionBid = new AuctionBid();
    
    auctionBid.setId(resultSet.getInt("auctionBid.id"));
    auctionBid.setAuctionId(resultSet.getInt("auctionBid.auction_id"));
    auctionBid.setObjectId(resultSet.getString("auctionBid.object_id"));
    auctionBid.setBidderId(resultSet.getInt("bidder_id"));
    auctionBid.setAuctionItemId(resultSet.getInt("auctionBid.ai_id"));
    auctionBid.setBidAmount(resultSet.getLong("auctionBid.bid_amount"));
    auctionBid.setTimestamp(resultSet.getLong("auctionBid.timestamp"));
    
    return auctionBid;
  }
  
  public static Map<Integer, AuctionBid> getAuctionItemIdToHighestAuctionBid(Statement statement, Set<Integer> auctionIdSet) throws SQLException {
    
    String sql = " SELECT *"
               + " FROM auctionBid"
               + " WHERE ai_id IN " + SQLUtil.buildListSQL(auctionIdSet, false, true)
               + " ORDER BY ai_id, bid_amount DESC";
    
    ResultSet resultSet = statement.executeQuery(sql);
    Map<Integer, AuctionBid> auctionItemIdToHighestAuctionBidMap = new HashMap<Integer, AuctionBid>();
    
    while(resultSet.next()) {
      
      AuctionBid auctionBid = getAuctionBid(resultSet);
      
      if(auctionItemIdToHighestAuctionBidMap.get(auctionBid.getAuctionItemId()) == null) {
        
        auctionItemIdToHighestAuctionBidMap.put(auctionBid.getAuctionItemId(), auctionBid);
      }
    }
    
    return auctionItemIdToHighestAuctionBidMap;
  }
}
