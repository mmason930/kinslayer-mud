package org.kinslayermud.object;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class EquipmentListingTypeStatic {

  static final J5EnumSet<EquipmentListingType> enumSet = new J5EnumSet<EquipmentListingType>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (EquipmentListingType enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum EquipmentListingType implements VEnum {

  armor(0, "Armor", "Armor"),
  weapon(1, "Weapon", "Weapons");
  

  private String singularName;
  private String pluralName;

  private EquipmentListingType (int id, String singularName, String pluralName) {
    this.singularName = singularName;
    this.pluralName = pluralName;
    EquipmentListingTypeStatic.addEnum(this, id, singularName);
  }

  public int value () { return EquipmentListingTypeStatic.enumSet.getValue(this); }
  public String toString () { return EquipmentListingTypeStatic.enumSet.toString(this); }

  public String getSingularName () { return singularName; }
  public String getPluralName () { return pluralName; }

  public static EquipmentListingType getEnum(int value) throws IndexOutOfBoundsException { return EquipmentListingTypeStatic.enumSet.getEnum(value); }
  public static Iterator<EquipmentListingType> getSetIterator () { return EquipmentListingTypeStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return EquipmentListingTypeStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return EquipmentListingTypeStatic.standardNameValueItemSet; }
};
