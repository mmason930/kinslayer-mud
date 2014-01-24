package org.kinslayermud.user.login;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class GatewayDescriptorTypeStatic {

  static final J5EnumSet<GatewayDescriptorType> enumSet = new J5EnumSet<GatewayDescriptorType>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (GatewayDescriptorType enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum GatewayDescriptorType implements VEnum {

  unknown(0, "Unknown"),
  rawTCP(1, "Raw TCP"),
  websocket(2, "WebSocket");

  private String standardName;

  private GatewayDescriptorType (int id, String standardName) {
    this.standardName = standardName;
    GatewayDescriptorTypeStatic.addEnum(this, id, standardName);
  }

  public int value () { return GatewayDescriptorTypeStatic.enumSet.getValue(this); }
  public String toString () { return GatewayDescriptorTypeStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static GatewayDescriptorType getEnum(int value) throws IndexOutOfBoundsException { return GatewayDescriptorTypeStatic.enumSet.getEnum(value); }
  public static Iterator<GatewayDescriptorType> getSetIterator () { return GatewayDescriptorTypeStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return GatewayDescriptorTypeStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return GatewayDescriptorTypeStatic.standardNameValueItemSet; }
};
