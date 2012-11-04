package org.kinslayermud.shipping;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class CarrierStatic {

  static final J5EnumSet<Carrier> enumSet = new J5EnumSet<Carrier>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (Carrier enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum Carrier implements VEnum {

  usps(0, "USPS"),
  fedex(1, "Fedex"),
  ups(2, "UPS");

  private String standardName;

  private Carrier (int id, String standardName) {
    this.standardName = standardName;
    CarrierStatic.addEnum(this, id, standardName);
  }

  public int value () { return CarrierStatic.enumSet.getValue(this); }
  public String toString () { return CarrierStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static Carrier getEnum(int value) throws IndexOutOfBoundsException { return CarrierStatic.enumSet.getEnum(value); }
  public static Iterator<Carrier> getSetIterator () { return CarrierStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return CarrierStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return CarrierStatic.standardNameValueItemSet; }
};
