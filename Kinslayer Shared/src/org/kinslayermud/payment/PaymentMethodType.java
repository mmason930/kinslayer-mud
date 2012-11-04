package org.kinslayermud.payment;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class PaymentMethodTypeStatic {

  static final J5EnumSet<PaymentMethodType> enumSet = new J5EnumSet<PaymentMethodType>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (PaymentMethodType enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum PaymentMethodType implements VEnum {

  paypalExpress(0, "Paypal Express");

  private String standardName;

  private PaymentMethodType (int id, String standardName) {
    this.standardName = standardName;
    PaymentMethodTypeStatic.addEnum(this, id, standardName);
  }

  public int value () { return PaymentMethodTypeStatic.enumSet.getValue(this); }
  public String toString () { return PaymentMethodTypeStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static PaymentMethodType getEnum(int value) throws IndexOutOfBoundsException { return PaymentMethodTypeStatic.enumSet.getEnum(value); }
  public static Iterator<PaymentMethodType> getSetIterator () { return PaymentMethodTypeStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return PaymentMethodTypeStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return PaymentMethodTypeStatic.standardNameValueItemSet; }
};
