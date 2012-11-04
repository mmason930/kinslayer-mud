package org.kinslayermud.saletransaction;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class SaleTransactionProductStatusStatic {

  static final J5EnumSet<SaleTransactionProductStatus> enumSet = new J5EnumSet<SaleTransactionProductStatus>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (SaleTransactionProductStatus enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum SaleTransactionProductStatus implements VEnum {

  pending(0, "Pending"),
  cancelled(1, "Cancelled"),
  shipped(2, "Shipped");

  private String standardName;

  private SaleTransactionProductStatus (int id, String standardName) {
    this.standardName = standardName;
    SaleTransactionProductStatusStatic.addEnum(this, id, standardName);
  }

  public int value () { return SaleTransactionProductStatusStatic.enumSet.getValue(this); }
  public String toString () { return SaleTransactionProductStatusStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static SaleTransactionProductStatus getEnum(int value) throws IndexOutOfBoundsException { return SaleTransactionProductStatusStatic.enumSet.getEnum(value); }
  public static Iterator<SaleTransactionProductStatus> getSetIterator () { return SaleTransactionProductStatusStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return SaleTransactionProductStatusStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return SaleTransactionProductStatusStatic.standardNameValueItemSet; }
};
