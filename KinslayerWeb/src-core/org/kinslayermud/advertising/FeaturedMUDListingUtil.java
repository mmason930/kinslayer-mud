package org.kinslayermud.advertising;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Collection;

import org.kinslayermud.util.SQLUtil;

public class FeaturedMUDListingUtil {

  public static FeaturedMUDListing getRandomFeaturedMUDListing(Statement statement, Collection<Integer> featuredMUDListingIdCollectionToExclude) throws SQLException {
    
    String sql = " SELECT *"
               + " FROM featuredMudListing"
               + " WHERE id NOT IN" + SQLUtil.buildListSQL(featuredMUDListingIdCollectionToExclude, false, true)
               + " ORDER BY RAND() LIMIT 1";
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
    
      FeaturedMUDListing listing = new FeaturedMUDListing();
      listing.retrieveFromResultSet(resultSet);
    }
    
    return null;
  }
}
