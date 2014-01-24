package org.kinslayermud.mob;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class SuperMobDifficultyStatic {

  static final J5EnumSet<SuperMobDifficulty> enumSet = new J5EnumSet<SuperMobDifficulty>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (SuperMobDifficulty enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum SuperMobDifficulty implements VEnum {

  easy(0, "Easy"),
  normal(1, "Normal"),
  challenging(2, "Challenging"),
  difficult(3, "Difficult");

  private String standardName;

  private SuperMobDifficulty (int id, String standardName) {
    this.standardName = standardName;
    SuperMobDifficultyStatic.addEnum(this, id, standardName);
  }

  public int value () { return SuperMobDifficultyStatic.enumSet.getValue(this); }
  public String toString () { return SuperMobDifficultyStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static SuperMobDifficulty getEnum(int value) throws IndexOutOfBoundsException { return SuperMobDifficultyStatic.enumSet.getEnum(value); }
  public static Iterator<SuperMobDifficulty> getSetIterator () { return SuperMobDifficultyStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return SuperMobDifficultyStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return SuperMobDifficultyStatic.standardNameValueItemSet; }
};
