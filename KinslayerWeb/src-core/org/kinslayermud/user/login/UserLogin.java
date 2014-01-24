package org.kinslayermud.user.login;

import java.util.Date;

import org.kinslayermud.misc.DataObjectWithIntId;

public class UserLogin extends DataObjectWithIntId {

  protected int userId;
  protected String host;
  protected Date loginDatetime;
  protected GatewayDescriptorType gatewayDescriptorType;
  
  public int getUserId() {
    return userId;
  }
  public void setUserId(int userId) {
    this.userId = userId;
  }
  public String getHost() {
    return host;
  }
  public void setHost(String host) {
    this.host = host;
  }
  public Date getLoginDatetime() {
    return loginDatetime;
  }
  public void setLoginDatetime(Date loginDatetime) {
    this.loginDatetime = loginDatetime;
  }
  public GatewayDescriptorType getGatewayDescriptorType() {
    return gatewayDescriptorType;
  }
  public void setGatewayDescriptorType(GatewayDescriptorType gatewayDescriptorType) {
    this.gatewayDescriptorType = gatewayDescriptorType;
  }
}
