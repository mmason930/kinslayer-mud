package org.kinslayermud.web.actions;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.net.URL;

import org.kinslayermud.cart.Cart;
import org.kinslayermud.image.ImageSize;
import org.kinslayermud.util.WebSiteUrlUtil;


public class GetImageAction extends StoreAction {

  public String execute(Cart  cart) {

    System.out.println("GetImagAction start!");

    response.setContentType("image/jpeg");

    // Send the file data
    try {

      String url = "file:" + WebSiteUrlUtil.getImagePath(ImageSize.full, "9781559346702");
      System.out.println("URL: " + url);
      BufferedInputStream inputStream = new BufferedInputStream(new URL(url).openStream());
      BufferedOutputStream outputStream = new BufferedOutputStream(response.getOutputStream());

      byte[] buffer = new byte[8192];
      while (true) {
        int numBytesRead = inputStream.read(buffer);
        if (numBytesRead < 0)
          break;

        outputStream.write(buffer, 0, numBytesRead);
      }

      outputStream.close();
      inputStream.close();

      return null;
    }
    catch(Throwable throwable) {

      throwable.printStackTrace();
    }

    return null;
  }

}
