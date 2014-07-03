package org.kinslayermud.character;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class GenderStatic {

  static final J5EnumSet<Gender> enumSet = new J5EnumSet<Gender>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (Gender enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum Gender implements VEnum {

  undefined(21, "Undefined"),
  neutral(0, "Neutral"),
  male(1, "Male"),
  female(2, "Female");
  
  private String standardName;

  private Gender (int id, String standardName) {
    this.standardName = standardName;
    GenderStatic.addEnum(this, id, standardName);
  }

  public int value () { return GenderStatic.enumSet.getValue(this); }
  public String toString () { return GenderStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static Gender getEnum(int value) throws IndexOutOfBoundsException { return GenderStatic.enumSet.getEnum(value); }
  public static Iterator<Gender> getSetIterator () { return GenderStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return GenderStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return GenderStatic.standardNameValueItemSet; }
};
