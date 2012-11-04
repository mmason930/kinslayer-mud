package org.kinslayermud.web.util;

import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;

public class HttpUtil {

  public static Cookie getCookieByName(HttpServletRequest request, String cookieName) {
    
    if(request.getCookies() == null) {
      
      return null;
    }
    
    for(Cookie cookie : request.getCookies()) {
      if(cookie.getName().equals(cookieName)) {
        return cookie;
      }
    }
    
    return null;
  }
}
