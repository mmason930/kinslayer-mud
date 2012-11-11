package org.kinslayermud.object;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class ObjectWearTypeStatic {

  static final J5EnumSet<ObjectWearType> enumSet = new J5EnumSet<ObjectWearType>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (ObjectWearType enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum ObjectWearType implements VEnum {

  take(0, "Take"),
  finger(1, "Finger"),
  neck(2, "Neck"),
  body(3, "Body"),
  head(4, "Head"),
  legs(5, "Legs"),
  feet(6, "Feet"),
  hands(7, "Hands"),
  arms(8, "Arms"),
  shield(9, "Shield"),
  about(10, "About"),
  waist(11, "Waist"),
  wrist(12, "Wrist"),
  wield(13, "Wield"),
  hold(14, "Hold"),
  back(15, "Back"),
  shoulders(16, "Shoulders"),
  ear(17, "Ear");

  private String standardName;

  private ObjectWearType (int id, String standardName) {
    this.standardName = standardName;
    ObjectWearTypeStatic.addEnum(this, id, standardName);
  }

  public int value () { return ObjectWearTypeStatic.enumSet.getValue(this); }
  public String toString () { return ObjectWearTypeStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static ObjectWearType getEnum(int value) throws IndexOutOfBoundsException { return ObjectWearTypeStatic.enumSet.getEnum(value); }
  public static Iterator<ObjectWearType> getSetIterator () { return ObjectWearTypeStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return ObjectWearTypeStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return ObjectWearTypeStatic.standardNameValueItemSet; }
};
