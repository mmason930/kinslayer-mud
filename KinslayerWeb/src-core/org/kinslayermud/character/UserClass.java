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

  warrior(    0, "Warrior"    , false),
  thief(      1, "Thief"      , false),
  ranger(     2, "Ranger"     , false),
  channeler(  3, "Channeler"  , false),
  fade(       4, "Fade"       , true),
  dreadlord(  5, "Dreadlord"  , true),
  blademaster(6, "Blademaster", true),
  greyman(    7, "Greyman"    , true),
  draghkar(   8, "Draghkar"   , true),
  dreadguard( 9, "Dreadguard" , false),
  ogier(     10, "Ogier"      , true),
  undefined( 21, "Undefined"  , false);
  
  private String standardName;
  private boolean isRemort;

  private UserClass (int id, String standardName, boolean isRemort) {
    this.standardName = standardName;
    this.isRemort = isRemort;
    UserClassStatic.addEnum(this, id, standardName);
  }

  public int value () { return UserClassStatic.enumSet.getValue(this); }
  public String toString () { return UserClassStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }
  public boolean isRemort () { return isRemort; }

  public static UserClass getEnum(int value) throws IndexOutOfBoundsException { return UserClassStatic.enumSet.getEnum(value); }
  public static Iterator<UserClass> getSetIterator () { return UserClassStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return UserClassStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return UserClassStatic.standardNameValueItemSet; }
};
