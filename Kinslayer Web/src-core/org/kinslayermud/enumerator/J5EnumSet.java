package org.kinslayermud.enumerator;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.TreeMap;

/**
 * EnumSet implementation for supporting Java language Enums as VEnums.
 */
public class J5EnumSet<Type extends Enum<Type>> implements VEnumSet {

  private TreeMap<Integer, Type> valueToEnumMap = new TreeMap<Integer, Type>();
  private Map<Type, Integer> enumToValueMap = new HashMap<Type, Integer>();

  public void registerEnum(Type enumValue, int value) {

    valueToEnumMap.put(value, enumValue);
    enumToValueMap.put(enumValue, value);
  }

  public Type getEnum(int value) throws IndexOutOfBoundsException {

    Type enumValue = valueToEnumMap.get(value);

    if (enumValue == null)
      throw new ArrayIndexOutOfBoundsException(value);

    return enumValue;
  }

  public VEnum get (int value) throws IndexOutOfBoundsException {

    return (VEnum) getEnum(value);
  }

  public int getValue(Enum enumValue) {

    return enumToValueMap.get(enumValue);
  }

  public String toString(Enum enumValue) {

    return enumToValueMap.get(enumValue).toString();
  }

  /**
   * Retrieves an order-constant iterator.  The order returned by the iterator
   * will remain contant unless the set is changed.<br>
   * <br>
   * Note: Constant-order is only guarenteed for a single run (and while set
   * is not mutated).
   */
  public Iterator<Type> iterator () {

    return valueToEnumMap.values().iterator();
  }

  public static <Type extends Enum<Type>> J5EnumSet<Type> checkSetupSetRegisterEnum(J5EnumSet<Type> enumSet, Type enumValue, int value) {

    if (enumSet == null)
      enumSet = new J5EnumSet<Type>();

    enumSet.registerEnum(enumValue, value);

    return enumSet;
  }
}