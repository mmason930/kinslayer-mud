package org.kinslayermud.character;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class UserClassStatic {

  static final J5EnumSet<UserClass> enumSet = new J5EnumSet<UserClass>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (UserClass enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum UserClass implements VEnum {

  warrior(0, "Warrior"),
  thief(1, "Thief"),
  ranger(2, "Ranger"),
  channeler(3, "Channeler"),
  fade(4, "Fade"),
  dreadlord(5, "Dreadlord"),
  blademaster(6, "Blademaster"),
  greyman(7, "Greyman"),
  draghkar(8, "Draghkar"),
  dreadguard(9, "Dreadguard"),
  ogier(10, "Ogier"),
  undefined(21, "Undefined");
  
  private String standardName;

  private UserClass (int id, String standardName) {
    this.standardName = standardName;
    UserClassStatic.addEnum(this, id, standardName);
  }

  public int value () { return UserClassStatic.enumSet.getValue(this); }
  public String toString () { return UserClassStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static UserClass getEnum(int value) throws IndexOutOfBoundsException { return UserClassStatic.enumSet.getEnum(value); }
  public static Iterator<UserClass> getSetIterator () { return UserClassStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return UserClassStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return UserClassStatic.standardNameValueItemSet; }
};
