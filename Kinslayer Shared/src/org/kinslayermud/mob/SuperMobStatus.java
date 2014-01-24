package org.kinslayermud.mob;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class SuperMobStatusStatic {

  static final J5EnumSet<SuperMobStatus> enumSet = new J5EnumSet<SuperMobStatus>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (SuperMobStatus enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum SuperMobStatus implements VEnum {

  open(0, "Open"),
  closed(1, "Closed");

  private String standardName;

  private SuperMobStatus (int id, String standardName) {
    this.standardName = standardName;
    SuperMobStatusStatic.addEnum(this, id, standardName);
  }

  public int value () { return SuperMobStatusStatic.enumSet.getValue(this); }
  public String toString () { return SuperMobStatusStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static SuperMobStatus getEnum(int value) throws IndexOutOfBoundsException { return SuperMobStatusStatic.enumSet.getEnum(value); }
  public static Iterator<SuperMobStatus> getSetIterator () { return SuperMobStatusStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return SuperMobStatusStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return SuperMobStatusStatic.standardNameValueItemSet; }
};
