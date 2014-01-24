package org.kinslayermud.object;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class ObjectExtraFlagStatic {

  static final J5EnumSet<ObjectExtraFlag> enumSet = new J5EnumSet<ObjectExtraFlag>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (ObjectExtraFlag enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum ObjectExtraFlag implements VEnum {

  noRent(0, "No Rent"),
  invisible(1, "Invisible"),
  noDrop(2, "No Drop"),
  ward(3, "Ward"),
  noSell(4, "No Sell"),
  chain(5, "Chain"),
  poison(6, "Poison"),
  chest(7, "Chest"),
  twoHanded(8, "Two Handed"),
  daggerSheath(9, "Dagger Sheath"),
  swordSheath(10, "Sword Sheath"),
  temp(11, "Temp"),
  noShow(12, "No Show"),
  clanEquipment(13, "Clan Equipment"),
  spearSheath(14, "Spear Sheath"),
  rare(15, "Rare"),
  trades(16, "Trades");
  

  private String standardName;

  private ObjectExtraFlag (int id, String standardName) {
    this.standardName = standardName;
    ObjectExtraFlagStatic.addEnum(this, id, standardName);
  }

  public int value () { return ObjectExtraFlagStatic.enumSet.getValue(this); }
  public String toString () { return ObjectExtraFlagStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static ObjectExtraFlag getEnum(int value) throws IndexOutOfBoundsException { return ObjectExtraFlagStatic.enumSet.getEnum(value); }
  public static Iterator<ObjectExtraFlag> getSetIterator () { return ObjectExtraFlagStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return ObjectExtraFlagStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return ObjectExtraFlagStatic.standardNameValueItemSet; }
};
