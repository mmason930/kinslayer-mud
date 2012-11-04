package org.kinslayermud.seller;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class SellerStatusStatic {

  static final J5EnumSet<SellerStatus> enumSet = new J5EnumSet<SellerStatus>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (SellerStatus enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum SellerStatus implements VEnum {

  pending(0, "Pending"),
  active(1, "Active"),
  paused(2, "Paused"),
  banned(3, "Banned");

  private String standardName;

  private SellerStatus (int id, String standardName) {
    this.standardName = standardName;
    SellerStatusStatic.addEnum(this, id, standardName);
  }

  public int value () { return SellerStatusStatic.enumSet.getValue(this); }
  public String toString () { return SellerStatusStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static SellerStatus getEnum(int value) throws IndexOutOfBoundsException { return SellerStatusStatic.enumSet.getEnum(value); }
  public static Iterator<SellerStatus> getSetIterator () { return SellerStatusStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return SellerStatusStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return SellerStatusStatic.standardNameValueItemSet; }
};
