package org.kinslayermud.object;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class ObjectWeaponTypeStatic {

  static final J5EnumSet<ObjectWeaponType> enumSet = new J5EnumSet<ObjectWeaponType>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (ObjectWeaponType enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum ObjectWeaponType implements VEnum {

  longBlade(0, "Long Blade", "Long Blades"),
  shortBlade(1, "Short Blade", "Short Blades"),
  club(2, "Club", "Clubs"),
  staff(3, "Staff", "Staves"),
  spear(4, "Spear", "Spears"),
  axe(5, "Axe", "Axes"),
  chain(6, "Chain", "Chains"),
  bow(7, "Bow", "Bows"),
  lance(8, "Lance", "Lances");
  
  private String singularName;
  private String pluralName;

  private ObjectWeaponType (int id, String singularName, String pluralName) {
    this.singularName = singularName;
    this.pluralName = pluralName;
    ObjectWeaponTypeStatic.addEnum(this, id, singularName);
  }

  public int value () { return ObjectWeaponTypeStatic.enumSet.getValue(this); }
  public String toString () { return ObjectWeaponTypeStatic.enumSet.toString(this); }

  public String getSingularName () { return singularName; }
  public String getPluralName () { return pluralName; }

  public static ObjectWeaponType getEnum(int value) throws IndexOutOfBoundsException { return ObjectWeaponTypeStatic.enumSet.getEnum(value); }
  public static Iterator<ObjectWeaponType> getSetIterator () { return ObjectWeaponTypeStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return ObjectWeaponTypeStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return ObjectWeaponTypeStatic.standardNameValueItemSet; }
};
