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

  public static List<AuctionItem> getActiveAuctionItems(Statement statement) throws SQLException {
    
    String criteria = "active = " + SQLUtil.encodeBooleanInt(true);
    return QueryUtil.retrieveDataObjectList(statement, "auctionItem", criteria, AuctionItem.class);
  }
  
  public static Map<Integer, AuctionBid> getAuctionItemIdToHighestAuctionBid(Statement statement, Set<Integer> auctionIdSet) throws SQLException {
    
    String sql = " SELECT *"
               + " FROM auctionBid"
               + " WHERE ai_id IN " + SQLUtil.buildListSQL(auctionIdSet, false, true)
               + " ORDER BY ai_id, bid_amount DESC";
    
    ResultSet resultSet = statement.executeQuery(sql);
    Map<Integer, AuctionBid> auctionItemIdToHighestAuctionBidMap = new HashMap<Integer, AuctionBid>();
    
    while(resultSet.next()) {
      
      AuctionBid auctionBid = new AuctionBid();
      auctionBid.retrieveFromResultSet(resultSet);
      
      if(auctionItemIdToHighestAuctionBidMap.get(auctionBid.getAuctionItemId()) == null) {
        
        auctionItemIdToHighestAuctionBidMap.put(auctionBid.getAuctionItemId(), auctionBid);
      }
    }
    
    return auctionItemIdToHighestAuctionBidMap;
  }
}
