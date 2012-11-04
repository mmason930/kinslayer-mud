package org.kinslayermud.util;

import org.kinslayermud.image.ImageSize;

public class WebSiteUrlUtil {

  protected static final String IMAGE_BASE_PATH = "/home/mymeatinyourmouth/images/";
  protected static final String IMAGE_BASE_URL = "/images/";
  
  public static String getImagePath(ImageSize imageSize, String productCode) {
    
    return IMAGE_BASE_PATH + imageSize.getDirectoryName() + "/"
    + productCode.substring(0,2) + "/"
    + productCode.substring(0,4) + "/"
    + productCode.substring(0,6) + "/"
    + productCode.substring(0,8) + "/"
    + productCode.substring(0,10)+ "/"
    + productCode + ".jpg";
  }
  
  public static String getImageUrl(ImageSize imageSize, String productCode) {
    
    return IMAGE_BASE_URL + imageSize.getDirectoryName() + "/"
    + productCode.substring(0,2) + "/"
    + productCode.substring(0,4) + "/"
    + productCode.substring(0,6) + "/"
    + productCode.substring(0,8) + "/"
    + productCode.substring(0,10)+ "/"
    + productCode + ".jpg";
  }
  
  public static String getSaleListingUrl(int saleListingId) {
    
    return "/ViewListing?SaleListingId=" + saleListingId;
  }
}
