package org.kinslayermud.user.login;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Date;

import org.kinslayermud.dbutils.DBObject;
import org.kinslayermud.misc.DataObjectWithIntId;

public class UserLogin extends DataObjectWithIntId implements DBObject {

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
  
  public void retrieveFromResultSet(ResultSet resultSet) throws SQLException {
    
    setId(resultSet.getInt("id"));
    setUserId(resultSet.getInt("user_id"));
    setHost(resultSet.getString("host"));
    setLoginDatetime(resultSet.getTimestamp("login_datetime"));
    setGatewayDescriptorType(GatewayDescriptorType.getEnum(resultSet.getInt("gateway_descriptor_type")));
  }
}
