package org.kinslayermud.image;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class ImageSizeStatic {

  static final J5EnumSet<ImageSize> enumSet = new J5EnumSet<ImageSize>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (ImageSize enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum ImageSize implements VEnum {

  full(0, "Full", "FULL");

  private String standardName;
  private String directoryName;

  private ImageSize (int id, String standardName, String directoryName) {
    this.standardName = standardName;
    this.directoryName = directoryName;
    ImageSizeStatic.addEnum(this, id, standardName);
  }

  public int value () { return ImageSizeStatic.enumSet.getValue(this); }
  public String toString () { return ImageSizeStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }
  public String getDirectoryName () { return directoryName; }
  
  public static ImageSize getEnum(int value) throws IndexOutOfBoundsException { return ImageSizeStatic.enumSet.getEnum(value); }
  public static Iterator<ImageSize> getSetIterator () { return ImageSizeStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return ImageSizeStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return ImageSizeStatic.standardNameValueItemSet; }
};
