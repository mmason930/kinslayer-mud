package org.kinslayermud.web.featuredmudlisting;

import java.util.Collection;
import java.util.HashSet;

import org.kinslayermud.advertising.FeaturedMUDListing;
import org.kinslayermud.util.MiscUtil;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;

import com.google.gson.Gson;

public class GetNextFeaturedMUDListingAction extends StandardAction {
  
  protected static final String FEATURED_MUD_LISTING_ID = "FeaturedMUDListingID";
  
  public String execute(WebSupport webSupport) throws Exception {

    Collection<Integer> featuredMUDListingIdCollectionToExclude = new HashSet<Integer>();
    if(request.getParameter(FEATURED_MUD_LISTING_ID) != null && MiscUtil.isValidIntString(request.getParameter(FEATURED_MUD_LISTING_ID))) {
      
      featuredMUDListingIdCollectionToExclude.add(Integer.valueOf(request.getParameter(FEATURED_MUD_LISTING_ID)));
    }
    
    FeaturedMUDListing featuredMUDListing = webSupport.getRandomFeaturedMUDListing(featuredMUDListingIdCollectionToExclude);
    
    response.getWriter().write(new Gson().toJson(featuredMUDListing));
    return null;
  }
}
