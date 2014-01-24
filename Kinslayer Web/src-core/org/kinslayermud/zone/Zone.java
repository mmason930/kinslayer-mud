package org.kinslayermud.zone;

import org.kinslayermud.misc.DataObjectWithIntId;

public class Zone extends DataObjectWithIntId {

  protected String name;
  protected int lifespan;
  protected int x;
  protected int y;
  protected int bottomRoomId;
  protected int topRoomId;
  protected int resetMode;
  protected String sunrise;
  protected String sunset;
  protected String builders;
  protected boolean closed;
  
  public String getName() {
    return name;
  }
  public void setName(String name) {
    this.name = name;
  }
  public int getLifespan() {
    return lifespan;
  }
  public void setLifespan(int lifespan) {
    this.lifespan = lifespan;
  }
  public int getX() {
    return x;
  }
  public void setX(int x) {
    this.x = x;
  }
  public int getY() {
    return y;
  }
  public void setY(int y) {
    this.y = y;
  }
  public int getBottomRoomId() {
    return bottomRoomId;
  }
  public void setBottomRoomId(int bottomRoomId) {
    this.bottomRoomId = bottomRoomId;
  }
  public int getTopRoomId() {
    return topRoomId;
  }
  public void setTopRoomId(int topRoomId) {
    this.topRoomId = topRoomId;
  }
  public int getResetMode() {
    return resetMode;
  }
  public void setResetMode(int resetMode) {
    this.resetMode = resetMode;
  }
  public String getSunrise() {
    return sunrise;
  }
  public void setSunrise(String sunrise) {
    this.sunrise = sunrise;
  }
  public String getSunset() {
    return sunset;
  }
  public void setSunset(String sunset) {
    this.sunset = sunset;
  }
  public String getBuilders() {
    return builders;
  }
  public void setBuilders(String builders) {
    this.builders = builders;
  }
  public boolean isClosed() {
    return closed;
  }
  public void setClosed(boolean closed) {
    this.closed = closed;
  }
}
