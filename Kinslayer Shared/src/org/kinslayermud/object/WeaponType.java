package org.kinslayermud.object;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class WeaponTypeStatic {

  static final J5EnumSet<WeaponType> enumSet = new J5EnumSet<WeaponType>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (WeaponType enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum WeaponType implements VEnum {

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

  private WeaponType (int id, String singularName, String pluralName) {
    
    this.singularName = singularName;
    this.pluralName = pluralName;
    WeaponTypeStatic.addEnum(this, id, singularName);
  }

  public int value () { return WeaponTypeStatic.enumSet.getValue(this); }
  public String toString () { return WeaponTypeStatic.enumSet.toString(this); }

  public String getSingularName () { return singularName; }
  public String getPluralName () { return pluralName; }

  public static WeaponType getEnum(int value) throws IndexOutOfBoundsException { return WeaponTypeStatic.enumSet.getEnum(value); }
  public static Iterator<WeaponType> getSetIterator () { return WeaponTypeStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return WeaponTypeStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return WeaponTypeStatic.standardNameValueItemSet; }
};
