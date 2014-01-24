package org.kinslayermud.object;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class ObjectTypeStatic {

  static final J5EnumSet<ObjectType> enumSet = new J5EnumSet<ObjectType>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (ObjectType enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum ObjectType implements VEnum {

  light(1, "Light"),
  money(2, "Money"),
  weapon(3, "Weapon"),
  armor(4, "Armor"),
  potion(5, "Potion"),
  other(6, "Other"),
  container(7, "Container"),
  note(8, "Note"),
  drinkContainer(9, "Drink Container"),
  key(10, "Key"),
  food(11, "Food"),
  pen(12, "Pen"),
  boat(13, "Boat"),
  fountain(14, "Fountain"),
  angreal(15, "Angreal"),
  chair(16, "Chair"),
  special(17, "Special"),
  lampPost(18, "Lamp Post"),
  skin(19, "Skin");

  private String standardName;

  private ObjectType (int id, String standardName) {
    this.standardName = standardName;
    ObjectTypeStatic.addEnum(this, id, standardName);
  }

  public int value () { return ObjectTypeStatic.enumSet.getValue(this); }
  public String toString () { return ObjectTypeStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static ObjectType getEnum(int value) throws IndexOutOfBoundsException { return ObjectTypeStatic.enumSet.getEnum(value); }
  public static Iterator<ObjectType> getSetIterator () { return ObjectTypeStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return ObjectTypeStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return ObjectTypeStatic.standardNameValueItemSet; }
};
