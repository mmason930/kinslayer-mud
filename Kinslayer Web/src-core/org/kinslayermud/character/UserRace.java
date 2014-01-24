package org.kinslayermud.character;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class UserRaceStatic {

  static final J5EnumSet<UserRace> enumSet = new J5EnumSet<UserRace>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (UserRace enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum UserRace implements VEnum {

  human(0, "Human"),
  trolloc(1, "Trolloc"),
  seanchan(2, "Seanchan"),
  aiel(3, "Aiel"),
  animal(4, "Animal"),
  other(5, "Other"),
  undefined(21, "Undefined");

  private String standardName;

  private UserRace (int id, String standardName) {
    this.standardName = standardName;
    UserRaceStatic.addEnum(this, id, standardName);
  }

  public int value () { return UserRaceStatic.enumSet.getValue(this); }
  public String toString () { return UserRaceStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static UserRace getEnum(int value) throws IndexOutOfBoundsException { return UserRaceStatic.enumSet.getEnum(value); }
  public static Iterator<UserRace> getSetIterator () { return UserRaceStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return UserRaceStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return UserRaceStatic.standardNameValueItemSet; }
};
