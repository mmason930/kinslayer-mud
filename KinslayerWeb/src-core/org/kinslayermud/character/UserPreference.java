package org.kinslayermud.character;

import java.util.Iterator;

import org.kinslayermud.enumerator.J5EnumSet;
import org.kinslayermud.enumerator.NameValueItemSet;
import org.kinslayermud.enumerator.VEnum;
import org.kinslayermud.enumerator.VEnumSet;

abstract class UserPreferenceStatic {

  static final J5EnumSet<UserPreference> enumSet = new J5EnumSet<UserPreference>();

  static final NameValueItemSet standardNameValueItemSet = new NameValueItemSet();

  static synchronized void addEnum (UserPreference enumRef, int enumID, String name) {
    enumSet.registerEnum(enumRef, enumID);
    standardNameValueItemSet.addItem(name, enumRef);
  }
}

public enum UserPreference implements VEnum {

  brief(0, "Brief"),
  compact(1, "Compact"),
  source(2, "Source"),
  notell(3, "No Tell"),
  disphp(4, "Display HP"),
  dispmana(5, "Display Mana"),
  dispmove(6, "Display Moves"),
  autoexit(7, "Auto Exit"),
  nohassle(8, "No Hassle"),
  quest(9, "Quest"),
  summonable(10, "Summonable"),
  norepeat(11, "No Repeat"),
  holylight(12, "Holy Light"),
  color1(13, "Color 1"),
  color2(14, "Color 2"),
  nowiz(15, "No Wiz"),
  log1(16, "Log1"),
  log2(17, "Log2"),
  nospeaks(18, "No Speaks"),
  sparetwo(19, "Spare Two"),
  sparethree(20, "Spare Three"),
  roomflags(21, "Room Flags"),
  kit(22, "Kit"),
  statted(23, "Statted"),
  unused1(24, "Unused1"),
  incognito(25, "Incognito"),
  tellMute(26, "Tell Mute"),
  sayMute(27, "Say Mute"),
  nonarr(28, "No Narrate"),
  nochat(29, "No Chat"),
  noyell(30, "No Yell"),
  noshout(31, "No Shout"),
  buildwalk(32, "Buildwalk"),
  spam(33, "Spam"),
  alert(34, "Alert"),
  gcomm1(35, "Gcomm 1"),
  gcomm2(36, "Gcomm 2"),
  autoscan(37, "Auto Scan"),
  noglobal(38, "No Global");
  
  private String standardName;

  private UserPreference (int id, String standardName) {
    this.standardName = standardName;
    UserPreferenceStatic.addEnum(this, id, standardName);
  }

  public int value () { return UserPreferenceStatic.enumSet.getValue(this); }
  public String toString () { return UserPreferenceStatic.enumSet.toString(this); }

  public String getStandardName () { return standardName; }

  public static UserPreference getEnum(int value) throws IndexOutOfBoundsException { return UserPreferenceStatic.enumSet.getEnum(value); }
  public static Iterator<UserPreference> getSetIterator () { return UserPreferenceStatic.enumSet.iterator(); }
  public static VEnumSet getSet () { return UserPreferenceStatic.enumSet; };

  public static NameValueItemSet getStandardNameValueItemSet () { return UserPreferenceStatic.standardNameValueItemSet; }
};
