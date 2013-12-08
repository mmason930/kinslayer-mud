package org.kinslayermud.auction;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

import org.kinslayermud.util.QueryUtil;
import org.kinslayermud.util.SQLUtil;

public class AuctionUtil {

  public static AuctionItem getAuctionItem(ResultSet resultSet) throws SQLException {
    
    AuctionItem auctionItem = new AuctionItem();

    auctionItem.setId(resultSet.getInt("auctionItem.id"));
    auctionItem.setAuctionId(resultSet.getInt("auctionItem.auction_id"));
    auctionItem.setObjectId(resultSet.getString("actionItem.object_id"));
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
}
