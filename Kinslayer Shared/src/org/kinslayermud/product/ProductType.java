package org.kinslayermud.product;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class ProductTypeStatic {

  static final J5EnumSet<ProductType> enumSet = new J5EnumSet<ProductType>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (ProductType enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum ProductType implements VEnum {

  book(0, "Book"),
  videoGameSoftware(2, "Video Game Software"),
  music(3, "Music"),
  movie(4, "Movie"),
  grocery(5, "Grocery");

  private String standardName;

  private ProductType (int id, String standardName) {
    this.standardName = standardName;
    ProductTypeStatic.addEnum(this, id, standardName);
  }

  public int value () { return ProductTypeStatic.enumSet.getValue(this); }
  public String toString () { return ProductTypeStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static ProductType getEnum(int value) throws IndexOutOfBoundsException { return ProductTypeStatic.enumSet.getEnum(value); }
  public static Iterator<ProductType> getSetIterator () { return ProductTypeStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return ProductTypeStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return ProductTypeStatic.standardNameValueItemSet; }
};
