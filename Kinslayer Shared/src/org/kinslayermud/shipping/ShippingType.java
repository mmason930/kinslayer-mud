package org.kinslayermud.shipping;

import java.math.BigDecimal;
import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class ShippingTypeStatic {

  static final J5EnumSet<ShippingType> enumSet = new J5EnumSet<ShippingType>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (ShippingType enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum ShippingType implements VEnum {

  standard(0, "Standard", 395, 4, 14),
  expedited(1, "Expedited", 695, 2, 5);

  private String standardName;
  private BigDecimal price;
  private int minimumDays;
  private int maximumDays;
  
  public BigDecimal getPrice() {
    
    return price;
  }  
  public int getMinimumDays() {
    
    return minimumDays;
  }
  public int getMaximumDays() {
    
    return maximumDays;
  }

  private ShippingType (int id, String standardName, int price, int minimumDays, int maximumDays) {
    this.standardName = standardName;
    this.price = BigDecimal.valueOf(price).movePointLeft(2);
    this.minimumDays = minimumDays;
    this.maximumDays = maximumDays;
    ShippingTypeStatic.addEnum(this, id, standardName);
  }

  public int value () { return ShippingTypeStatic.enumSet.getValue(this); }
  public String toString () { return ShippingTypeStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static ShippingType getEnum(int value) throws IndexOutOfBoundsException { return ShippingTypeStatic.enumSet.getEnum(value); }
  public static Iterator<ShippingType> getSetIterator () { return ShippingTypeStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return ShippingTypeStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return ShippingTypeStatic.standardNameValueItemSet; }
};
