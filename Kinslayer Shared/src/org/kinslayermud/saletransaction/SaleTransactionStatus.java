package org.kinslayermud.saletransaction;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class SaleTransactionStatusStatic {

  static final J5EnumSet<SaleTransactionStatus> enumSet = new J5EnumSet<SaleTransactionStatus>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (SaleTransactionStatus enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum SaleTransactionStatus implements VEnum {

  complete(0, "Complete");

  private String standardName;

  private SaleTransactionStatus (int id, String standardName) {
    this.standardName = standardName;
    SaleTransactionStatusStatic.addEnum(this, id, standardName);
  }

  public int value () { return SaleTransactionStatusStatic.enumSet.getValue(this); }
  public String toString () { return SaleTransactionStatusStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static SaleTransactionStatus getEnum(int value) throws IndexOutOfBoundsException { return SaleTransactionStatusStatic.enumSet.getEnum(value); }
  public static Iterator<SaleTransactionStatus> getSetIterator () { return SaleTransactionStatusStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return SaleTransactionStatusStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return SaleTransactionStatusStatic.standardNameValueItemSet; }
};
