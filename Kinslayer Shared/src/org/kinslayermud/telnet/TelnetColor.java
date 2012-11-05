package org.kinslayermud.telnet;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class TelnetColorStatic {

  static final J5EnumSet<TelnetColor> enumSet = new J5EnumSet<TelnetColor>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (TelnetColor enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum TelnetColor implements VEnum {

  normal(0, "Normal", null, 0),
  bold(0, "Bold", null, 1),
  red(0, "Red", "800000", 31),
  green(0, "Green", "00B300", 32),
  yellow(0, "Yellow", "808000", 33),
  blue(0, "Blue", "000080", 34),
  magenta(0, "Magenta", "800080", 35),
  cyan(0, "Cyan", "008080", 36),
  white(0, "White", "FFFFFF", 37);
  
  private String standardName;
  private String hexCode;
  private int colorCode;

  private TelnetColor (int id, String standardName, String hexCode, int colorCode) {
    this.standardName = standardName;
    this.hexCode = hexCode;
    this.colorCode = colorCode;
    TelnetColorStatic.addEnum(this, id, standardName);
  }

  public int value () { return TelnetColorStatic.enumSet.getValue(this); }
  public String toString () { return TelnetColorStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }
  public String getHexCode() {
    
    return hexCode;
  }
  
  public int getColorCode() {
    
    return colorCode;
  }
  
  public static TelnetColor getTelnetColorByColorCode(int colorCode) {
    
    Iterator<TelnetColor> iter = TelnetColor.getSetIterator();
    while(iter.hasNext()) {
      
      TelnetColor telnetColor = iter.next();
      
      if(telnetColor.getColorCode() == colorCode) {
        
        return telnetColor;
      }
    }
    
    return null;
  }

  public static TelnetColor getEnum(int value) throws IndexOutOfBoundsException { return TelnetColorStatic.enumSet.getEnum(value); }
  public static Iterator<TelnetColor> getSetIterator () { return TelnetColorStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return TelnetColorStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return TelnetColorStatic.standardNameValueItemSet; }
};
