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

  take(0, "Take", "Take", "Misc Items", false),
  finger(1, "Fingers", "Finger", "Rings", true),
  neck(2, "Neck", "Neck", "Neck Trinkets", true),
  body(3, "Body", "Body", "Upper Body Gear", true),
  head(4, "Head", "Head", "Head Gear", true),
  legs(5, "Legs", "Leg", "Leg Wear", true),
  feet(6, "Feet", "Foot", "Foot Wear", true),
  hands(7, "Hands", "Hand", "Hand Gear", true),
  arms(8, "Arms", "Arm", "Arm Wear", true),
  shield(9, "Shields", "Shield", "Shields", true),
  about(10, "About", "About", "Cloaks & Capes", true),
  waist(11, "Waist", "Waist", "Belts", true),
  wrist(12, "Wrists", "Wrist", "Wrist Trinkets", true),
  wield(13, "Wield", "Wield", "Weapons", false),
  hold(14, "Hold", "Hold", "Held Items", false),
  back(15, "Back", "Back", "Back Wear", false),
  shoulders(16, "Shoulders", "Shoulder", "Shoulder Wear", true),
  ear(17, "Ears", "Ear", "Ear Trinkets", true);

  private String pluralTerm;
  private String singularTerm;
  private String displayTerm;
  private boolean showOnWebsite;

  private ObjectWearType (int id, String pluralTerm, String singularTerm, String displayTerm, boolean showOnWebsite) {
    this.pluralTerm = pluralTerm;
    this.singularTerm = singularTerm;
    this.displayTerm = displayTerm;
    this.showOnWebsite = showOnWebsite;
    ObjectWearTypeStatic.addEnum(this, id, pluralTerm);
  }

  public int value () { return ObjectWearTypeStatic.enumSet.getValue(this); }
  public String toString () { return ObjectWearTypeStatic.enumSet.toString(this); }

  public String getPluralTerm () { return pluralTerm; }
  public String getSingularTerm () { return singularTerm; }
  public String getDisplayTerm () { return displayTerm; }
  public boolean getShowOnWebsite () { return showOnWebsite; }

  public static ObjectWearType getEnum(int value) throws IndexOutOfBoundsException { return ObjectWearTypeStatic.enumSet.getEnum(value); }
  public static Iterator<ObjectWearType> getSetIterator () { return ObjectWearTypeStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return ObjectWearTypeStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return ObjectWearTypeStatic.standardNameValueItemSet; }
};
