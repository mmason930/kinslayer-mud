package org.kinslayermud.util;

import java.awt.Color;
import java.awt.Image;
import java.awt.MediaTracker;
import java.awt.Panel;
import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.StringSelection;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.Array;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.net.URL;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;
import java.text.DecimalFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.Random;
import java.util.Set;
import java.util.TimeZone;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.xml.bind.DatatypeConverter;

import org.kinslayermud.genericaccessor.GenericAccessor;

/** Contains miscellaneous utility functions. */
public abstract class MiscUtil {

  private static final String[] months = {
      "January", "February", "March", "April",
      "May", "June", "July", "August",
      "September", "October", "November", "December"
  };

  private static char[] defaultRandomIDCharacterSet = null;
  private static char[] numericRandomIDCharacterSet = null;

  public static String md5(String str) throws NoSuchAlgorithmException, UnsupportedEncodingException {
    
    MessageDigest messageDigest = MessageDigest.getInstance("MD5");
    String md5Hex = new BigInteger(1, messageDigest.digest(str.getBytes("UTF-8"))).toString(16);
    
    if(md5Hex.length() != 32)
      return "0" + md5Hex;
    
    return md5Hex;
  }
  
  /**
   * Locates and opens the specified file as an InputStream.  If the specified
   * file is not found or fails to open, an exception is thrown.
   * 
   * @param url A path specifying the resource location within the classpath.
   */
  public static InputStream getResourceStream (String url) throws Exception {

    if (url.startsWith("/")) {
      InputStream ins = MiscUtil.class.getResourceAsStream(url);
      if (ins == null)
        throw new FileNotFoundException("Failed to find resource \"" + url + "\"");
      return ins;
    }

    URL urlObject = new URL(url);
    return urlObject.openStream();
  }

  /**
   * Locates and loads the specified file as a string.  If the specified file
   * is not found or fails to load, an exception is thrown.
   * 
   * @param url A path specifying the resource location within the classpath.
   */
  public static String loadStringResource (String url) throws Exception {
    InputStream ins = getResourceStream(url);
    InputStreamReader reader = new InputStreamReader(ins); 
    StringBuffer sbuf = new StringBuffer();
    char[] cbuf = new char[8192];
    while (true) {
      int num = reader.read(cbuf, 0, cbuf.length);
      if (num < 0)
        break;
      sbuf.append(cbuf, 0, num);
    }

    return sbuf.toString();
  }

  /**
   * Locates and loads the specified property files.  If the specified file
   * is not found or fails to load, an exception is thrown.
   * 
   * @param url A path specifying the resource location within the classpath.
   */
  public static Properties loadPropertiesResource (String url) throws Exception {
    InputStream ins = getResourceStream(url);
    Properties properties = new Properties();
    properties.load(ins);
    ins.close();
      
    return properties;
  }

  /**
   * Locates and loads the specified image resource.  If the specified image
   * is not found or fails to load, an exception is thrown.
   * 
   * @param url A path specifying the resource location within the classpath.
   */ 
  public static Image loadImageResource (String url) throws Exception {
    URL imageURL = MiscUtil.class.getResource(url);
    if (imageURL == null)
      throw new FileNotFoundException("Failed to find resource \"" + url + "\"");

    Toolkit toolkit = Toolkit.getDefaultToolkit();
    Image image = toolkit.createImage(imageURL);

    // Load the image now so all the data is immediately available
    MediaTracker tracker = new MediaTracker(new Panel());
    tracker.addImage(image, 0);
    tracker.waitForAll();

    return image;
  }

  /**
   * Locates and loads the specified image resource.  If the specified image
   * is not found or fails to load, defaultImage is returned instead.
   * 
   * @param url A path specifying the resource location within the classpath.
   */
  public static Image loadImageResource (String url, Image defaultImage) {
    try {
      return loadImageResource(url);
    }
    catch (Exception e) {
      return defaultImage;
    }
  }

  /** Place text in the clipboard (so it can be pasted elsewhere). */
  public static void setClipboard (String text) {
    Clipboard clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
    StringSelection contents = new StringSelection(text);
    clipboard.setContents(contents, null);
  }

  /**
   * Checks if the String represents a valid integer value.  If this method
   * returns true, then (for example) the string can be passed to Integer.parseInt
   * without an exception being thrown.
   */
  public static boolean isValidIntString (String valueString) {
    try {
      Integer.parseInt(valueString);
      return true;
    }
    catch (NumberFormatException nfe) {
      return false;
    }
  }

  /**
   * Checks if the String represents a valid long value.  If this method
   * returns true, then (for example) the string can be passed to Long.parseLong
   * without an exception being thrown.
   */
  public static boolean isValidLongString (String valueString) {
    try {
      Long.parseLong(valueString);
      return true;
    }
    catch (NumberFormatException nfe) {
      return false;
    }
  }

  /**
   * Checks if the String represents a valid double value.  If this method
   * returns true, then (for example) the string can be passed to Double.parseDouble
   * without an exception being thrown.
   */
  public static boolean isValidDoubleString (String valueString) {
    try {
      Double.parseDouble(valueString);
      return true;
    }
    catch (NumberFormatException nfe) {
      return false;
    }
  }
  
  /**
   * Checks if the String represents a valid integer value.  If this method
   * returns true, then (for example) the string can be passed to Integer.parseInt
   * without an exception being thrown.
   */
  public static boolean isValidNumericString (String valueString) {
    try {
      Long.parseLong(valueString);
      return true;
    }
    catch (NumberFormatException nfe) {
      return false;
    }
  }

  /**
   * Cleans a String of any characters that would cause it to represent a
   * non-well-formed int and then parses the String or returns the defaultValue
   * if the String is of zero length.<br>
   * <br>
   * NOTE: this method will not throw a parsing based exception.  This is useful
   * for parsing strings that should not contain anything but an integer value.
   * 
   * @param valueString The string to clean.
   * @param defaultValue The default value to use if the string is empty.
   * @param signed Whether to keep a negative prefix.
   */
  public static int cleanParseInt (String valueString, int defaultValue, boolean signed) {
    try {
      valueString = cleanNumericString(valueString, signed, false); // Remove non-numeric elements
      return (valueString.length() > 0) ? Integer.parseInt(valueString) : defaultValue;
    }
    catch(Exception e) {
      return defaultValue;
    }
  }

  /** Same as calling cleanParseInt(valueString, defaultValue, true) */
  public static int cleanParseInt (String valueString, int defaultValue) {
    return cleanParseInt(valueString, defaultValue, true);
  }

  /**
   * Cleans a String of any characters that would cause it to represent a
   * non-well-formed int and then parses the String or returns the defaultValue
   * if the String is of zero length.<br>
   * <br>
   * NOTE: this method will not throw a parsing based exception.  This is useful
   * for parsing strings that should not contain anything but an integer value.
   * 
   * @param valueString The string to clean.
   * @param defaultValue The default value to use if the string is empty.
   * @param signed Whether to keep a negative prefix.
   */
  public static long cleanParseLong (String valueString, long defaultValue, boolean signed) {
    valueString = cleanNumericString(valueString, signed, false); // Remove non-numeric elements
    return (valueString.length() > 0) ? Long.parseLong(valueString) : defaultValue;
  }

  /** Same as calling cleanParseLong(valueString, defaultValue, true) */
  public static long cleanParseLong (String valueString, long defaultValue) {
    return cleanParseLong(valueString, defaultValue, true);
  }

  /**
   * Cleans a String of any characters that would cause it to represent a
   * non-well-formed decimal number and then parses the String or returns the
   * defaultValue if the String is of zero length.<br>
   * <br>
   * NOTE: this method will not throw a parsing based exception.  This is useful
   * for parsing strings that should not contain anything but an decimal value.
   * 
   * @param valueString The string to clean.
   * @param defaultValue The default value to use if the string is empty.
   * @param signed Whether to keep a negative prefix.
   */
  public static BigDecimal cleanParseBigDecimal (String valueString, BigDecimal defaultValue, boolean signed) {
    valueString = cleanNumericString(valueString, signed, true); // Remove non-numeric elements
    return (valueString.length() > 0) ? new BigDecimal(valueString) : defaultValue;
  }

  /** Same as calling cleanParseBigDecimal(valueString, defaultValue, true) */
  public static BigDecimal cleanParseBigDecimal (String valueString, BigDecimal defaultValue) {
    return cleanParseBigDecimal(valueString, defaultValue, true);
  }

  /**
   * Cleans a String of any characters that would cause it to represent a
   * non-well-formed decimal number and then parses the String or returns the
   * defaultValue if the String is of zero length.<br>
   * <br>
   * NOTE: this method will not throw a parsing based exception.  This is useful
   * for parsing strings that should not contain anything but an decimal value.
   * 
   * @param valueString The string to clean.
   * @param defaultValue The default value to use if the string is empty.
   * @param signed Whether to keep a negative prefix.
   */
  public static int cleanParseFixedPointInt (String valueString, int defaultValue, boolean signed) {
    valueString = cleanNumericString(valueString, signed, true); // Remove non-numeric elements
    if (valueString.length() > 0) {
      BigDecimal valueBigDecimal = new BigDecimal(valueString);
      return valueBigDecimal.movePointRight(2).intValue();
    }
    else {
      return defaultValue;
    }
  }

  /** Same as calling cleanParseBigDecimal(valueString, defaultValue, true) */
  public static int cleanParseFixedPointInt (String valueString, int defaultValue) {
    return cleanParseFixedPointInt(valueString, defaultValue, true);
  }

  /**
   * Cleans non-numeric characters from a string.<br>
   * <br>
   * NOTE: Resultant string may be empty.
   * 
   * @param text The string to clean.
   * @param allowSign Whether the sign should be kept, if found.
   * @param allowDecimal Whether the decimal point should be kept, if found
   * 
   * @return A string containing only digits and possible a prefix sign and/or decimal-point.
   */
  public static String cleanNumericString (String text, boolean allowSign, boolean allowDecimal) {
    if (text == null)
      return "";

    int textLength = text.length();
    StringBuffer strBuf = new StringBuffer(textLength);
    for (int i = 0; i < textLength; i++) {
      char ch = text.charAt(i);
      if (Character.isDigit(ch)
          || (ch == '-' && allowSign && strBuf.length() == 0)
          || (ch == '.' && allowDecimal && strBuf.indexOf(".") < 0)) {
        strBuf.append(ch);
      }
    }

    // If the resulting string is "-" return the empty string instead
    if (strBuf.length() == 1 && strBuf.charAt(0) == '-')
      strBuf.setLength(0);

    return strBuf.toString();
  }

  /** Same as calling cleanNumericString(text, signed, false) */
  public static String cleanNumericString (String text, boolean allowSign) {
    return cleanNumericString(text, allowSign, false);
  }

  public static boolean strictParseBoolean (String text, boolean allowMixedCase) throws IllegalArgumentException {
    if (allowMixedCase)
      text = text.toLowerCase();

    if (text.equals("true"))
      return true;
    else if (text.equals("false"))
      return false;
    else
      throw new IllegalArgumentException("Invalid value: " + text);
  }

  public static boolean strictParseBooleanInt (String text) throws IllegalArgumentException {
    if (text == null || text.equals(""))
      return false;

    if (text.equals("1"))
      return true;
    else if (text.equals("0"))
      return false;
    else
      throw new IllegalArgumentException("Invalid value: " + text);
  }

  public static Boolean strictParseBooleanObjInt (String text) throws IllegalArgumentException {
    if (text == null || text.equals(""))
      return null;

    if (text.equals("1"))
      return true;
    else if (text.equals("0"))
      return false;
    else
      throw new IllegalArgumentException("Invalid value: " + text);
  }

  public static String formatNumbericValueWithCommas (long value) {
    DecimalFormat decimalFormat = new DecimalFormat("##,###,###,###,###,###,##0");
    return decimalFormat.format(value);
  }

  public static String formatNumbericValueWithCommas (int value) {
    DecimalFormat decimalFormat = new DecimalFormat("##,###,###,###,###,###,##0");
    Integer integer = new Integer(value);
    return decimalFormat.format(integer.longValue());
  }

  /**
   * Formats a money value to contain a dollar sign, comma separated digit
   * triplets, and a two digit decimal portion.  The input is of penny
   * value (i.e. dollar value * 100).
   */
  public static String formatFixedPointMoneyValue (int money) {
    BigDecimal moneyValue = BigDecimal.valueOf(money, 2);
    return formatMoneyValue(moneyValue);
  }

  /**
   * Formats a money value to contain a dollar sign, comma separated digit
   * triplets, and a two digit decimal portion.  Negative values will appear
   * with a prefixed negative sign.
   */
  public static String formatMoneyValue (BigDecimal moneyValue) {
    // Note: to ensure thread-safety we create a new instance of DecimalFormat
    DecimalFormat moneyFormat = new DecimalFormat("$###,###,###,##0.00");
    return moneyFormat.format(moneyValue);
  }

  /**
   * Formats a money value to contain a dollar sign, comma separated digit
   * triplets, and a two digit decimal portion.  Negative values will appear
   * with a prefixed negative sign.
   */
  public static String formatMoneyValueExtended (BigDecimal moneyValue) {
    // Note: to ensure thread-safety we create a new instance of DecimalFormat
    DecimalFormat moneyFormat = new DecimalFormat("$###,###,###,##0.0000");
    return moneyFormat.format(moneyValue);
  }

  /**
   * Formats a money value to contain a dollar sign, comma separated digit
   * triplets, and a two digit decimal portion.  Negative values will appear
   * with parenthesis surrounding the value.  The input is of penny
   * value (i.e. dollar value * 100).
   */
  public static String formatFixedPointLedgerMoneyValue (int moneyValue) {
    BigDecimal moneyValueBD = BigDecimal.valueOf(moneyValue, 2);
    return formatLedgerMoneyValue(moneyValueBD);
  }

  /**
   * Formats a value to contain a two digit decimal portion.  
   * The input is of penny value (i.e. dollar value * 100).
   */
  public static String formatFixedPointValue (int value) {
    BigDecimal valueBD = BigDecimal.valueOf(value, 2);
    // Note: to ensure thread-safety we create a new instance of DecimalFormat
    DecimalFormat moneyFormat = new DecimalFormat("########0.00");
    return moneyFormat.format(valueBD);
  }

  /**
   * Formats a money value to contain a dollar sign, comma separated digit
   * triplets, and a two digit decimal portion.  Negative values will appear
   * with parenthesis surrounding the value.
   */
  public static String formatLedgerMoneyValue (BigDecimal moneyValue) {
    // Note: to ensure thread-safety we create a new instance of DecimalFormat
    DecimalFormat moneyFormat = new DecimalFormat("$###,###,###,##0.00;'('$###,###,###,##0.00')'");
    return moneyFormat.format(moneyValue);
  }

  /**
   * Formats a percent value to one decimal point
   */   
  public static String formatPercentValue (double value) {
    return formatPercentValue(value, 1);
  }

  public static String formatPercentValueNoDecimal (double value) {
    return formatPercentValue(value, 0);
  }

  public static String formatPercentValue (double value, int decimal_places) {
    String decimal_portion = "";
    if(decimal_places > 0) {
      decimal_portion = ".";
      for(int i=0; i<decimal_places; i++) {
        decimal_portion += "0";        
      }      
    }

    DecimalFormat percentFormat = new DecimalFormat("##0" + decimal_portion + "%");
    return percentFormat.format(value);
  }

  public static String formatPercentValue (BigDecimal value, int decimal_places) {
    String decimal_portion = "";
    if(decimal_places > 0) {
      decimal_portion = ".";
      for(int i=0; i<decimal_places; i++) {
        decimal_portion += "0";        
      }      
    }

    DecimalFormat percentFormat = new DecimalFormat("##0" + decimal_portion);
    return percentFormat.format(value) + "%";
  }

  /**
   * Formats a double with two decimal places
   */   
  public static String formatDoubleToTwoDecimalPlaces (double value) {
    DecimalFormat percentFormat = new DecimalFormat("###,###,###,##0.00");
    return percentFormat.format(value);
  }

  /**
   * sets Date to GMT
   */
  public static Date setDateToGMT(Date date) {
    GregorianCalendar calendar = new GregorianCalendar();
    calendar.setTime(date);

    TimeZone localTimeZone = TimeZone.getDefault();
    int offset = localTimeZone.getOffset(
      calendar.get(GregorianCalendar.ERA),
      calendar.get(GregorianCalendar.YEAR),
      calendar.get(GregorianCalendar.MONTH),
      calendar.get(GregorianCalendar.DATE),
      calendar.get(GregorianCalendar.DAY_OF_WEEK),
      calendar.get(GregorianCalendar.MILLISECOND)
    );

    long convertedTime = date.getTime() - offset;
    return new Date(convertedTime);
  }

  /** Formats the date in the form MM */
  public static String formatDateMM (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MM");
    return dateFormat.format(date);
  }

  /** Formats the date in the form dd */
  public static String formatDateDD (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("dd");
    return dateFormat.format(date);
  }

  /** Formats the date in the form MM/dd/yy */
  public static String formatDateMMsDDsYY (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MM/dd/yy");
    return dateFormat.format(date);
  }

  /** Formats the date in the form MM/dd/yyyy */
  public static String formatDateMMsDDsYYYY (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MM/dd/yyyy");
    return dateFormat.format(date);
  }

  public static String formatDateMMsDDsYYYYsHHcMMcSS (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MM/dd/yyyy HH:mm:ss");
    return dateFormat.format(date);
  }
  
  public static String formatDateMMfsDDfsYY (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MM/dd/yy");
    return dateFormat.format(date);
  }

  /** Formats the date in the form m/dd */
  public static String formatDateMsDs (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("M/d");
    return dateFormat.format(date);
  }   

  public static String formatDateYYYYdMMdDDThhcmmcss (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd");
    SimpleDateFormat dateFormat2 = new SimpleDateFormat("HH:mm:ss");
    return dateFormat.format(date) + "T" + dateFormat2.format(date) + "-05:00";
  }
  
  public static String formatDateYYYYdMMdDDThhcmmcssZ (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd");
    SimpleDateFormat dateFormat2 = new SimpleDateFormat("HH:mm:ss");
    return dateFormat.format(date) + "T" + dateFormat2.format(date) + "Z";
  }
  
  /** Formats the date in the form yyyy-MM-dd HH:mm:ss.S */
  public static String formatDateYYYYdMMdDDsHHcMMcSSdS (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.S");
    return dateFormat.format(date);
  }
   
  /** Formats the date in the form yyyyMMddHHmmss */
  public static String formatDateYYYYMMDDHHMMSS (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyyMMddHHmmss");
    return dateFormat.format(date);
  }

  /** Formats the date in the form yyyyMMddHHmmssSSS */
  public static String formatDateYYYYMMDDHHMMSSSSS (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyyMMddHHmmssSSS");
    return dateFormat.format(date);
  }

  public static Date parseDateIso8601 (String dateString) throws IllegalArgumentException {
    return DatatypeConverter.parseDateTime(dateString).getTime();
  }
  
  public static Date parseDateYYYYMMDDHHMMSS(String dateString) throws ParseException {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyyMMddHHmmss");
    return dateFormat.parse(dateString);
  }

  public static Date parseDateYYYYdMMdDDsHHcMMcSSdS (String dateString) throws ParseException{
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.S");
    return dateFormat.parse(dateString);
  }

  public static Date parseDateYYYYdMMdDDsHHcMMcSS (String dateString) throws ParseException{
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
    return dateFormat.parse(dateString);
  }

  public static Date parseDateYYYYdMMdDDThhcmmcssZ (String dateString) throws ParseException {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss'Z'");
    return dateFormat.parse(dateString);
  }

  public static Date parseDateYYYYMMDD (String dateString) throws ParseException{
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyyMMdd");
    return dateFormat.parse(dateString);
  }

  public static Date parseDateYYYYfsMMfsDD (String dateString) throws ParseException{
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd");
    return dateFormat.parse(dateString);
  }

  public static Date parseDateMMfsDDfsYYYY (String dateString) throws ParseException{
    SimpleDateFormat dateFormat = new SimpleDateFormat("MM/dd/yyyy");
    return dateFormat.parse(dateString);
  }

  public static Date parseDateMMfsDDfsYYYYsHHcMM (String dateString) throws ParseException{
    SimpleDateFormat dateFormat = new SimpleDateFormat("MM/dd/yyyy H:mm");
    return dateFormat.parse(dateString);
  }
  
  public static Date parseDateMMfsDDfsYY (String dateString) throws ParseException{
    SimpleDateFormat dateFormat = new SimpleDateFormat("MM/dd/yy");
    return dateFormat.parse(dateString);
  }

  /** Formats the date in the form dd-MMM (10-Dec) */
  public static String formatDateDDdMMM (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("dd-MMM");
    return dateFormat.format(date);
  }

  /** Formats the date in the form dd-MMM-YY (10-Dec-04) */
  public static String formatDateDDdMMMdYY (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("dd-MMM-yy");
    return dateFormat.format(date);
  }

  /** Formats the date in the form dd-MMM-yyyy (10-Dec-2004) */  
  public static String formatDateDDdMMMdYYYY (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("dd-MMM-yyyy");
    return dateFormat.format(date);
  }

  /** Formats the date in the form dd-MMM h:mma (10-Dec 3:00pm) */
  public static String formatDateDDdMMMsHcMMam (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("dd-MMM h:mm");
    return dateFormat.format(date) + formatDateAMPMLowerCase(date);
  }

  /** Formats the date in the form dd-MMM-YY (10-Dec-04) */
  public static String formatDateDDdMMMdYYsHcMMam (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("dd-MMM-yy h:mm");
    return dateFormat.format(date) + formatDateAMPMLowerCase(date);
  }

  /** Formats the date in the form dd-MM-YY h:mm:ss a(10-12-04 1:00:00 am) */
  public static String formatDateDDdMMMdYYsHcMMcSSsam (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("dd-MM-yy h:mm:ss");
    return dateFormat.format(date) + formatDateAMPMLowerCase(date);
  }

  public static String formatDateAMPMLowerCase (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("a");
    return dateFormat.format(date).toLowerCase();
  }

  /** Formats the date in the form dd-MMMM YYYY (10-December 2004) */
  public static String formatDateDDdMMMMsYYYY (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("dd-MMMM yyyy");
    return dateFormat.format(date);
  }
   
  /** Formats the date in the form MMMM YYYY (December 2004) */
  public static String formatDateMMMMsYYYY (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MMMM yyyy");
    return dateFormat.format(date);
  }
  
  /** Formats the date in the form MMM dd YYYY (Dec 10, 2004) */
  public static String formatDateMMMsDDcsYYYY (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MMM dd, yyyy");
    return dateFormat.format(date);
  }

  /** Formats the date in the form MMM dd YYYY hh:mm:ss (Dec 10, 2004 23:59:59) */
  public static String formatDateMMMsDDcsYYYYsHHsMMsSS (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MMM dd, yyyy hh:mm:ss");
    return dateFormat.format(date);
  }  

  /** Formats the date in the form MMMM dd YYYY (December 10, 2004) */
  public static String formatDateMMMMsDDcsYYYY (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MMMM dd, yyyy");
    return dateFormat.format(date);
  }

  /** Formats the date in the form MMMM d YYYY (December 10, 2004) */
  public static String formatDateMMMMsDscsYYYY (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MMMM ");
    SimpleDateFormat dateFormat2 = new SimpleDateFormat("d");
    SimpleDateFormat dateFormat3 = new SimpleDateFormat(", yyyy");
    return dateFormat.format(date) + dateFormat2.format(date) + getNumberSuffix(dateFormat2.format(date)) + dateFormat3.format(date);
  }

  /** Formats the date in the form "MMM d" (Jan 6) */
  public static String formatDateMMMsD (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MMM d");
    return dateFormat.format(date);
  }

  /** Formats the date in the form "MMM d(suf)" (Jan 6) */
  public static String formatDateMMMsDu (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MMM ");
    SimpleDateFormat dateFormat2 = new SimpleDateFormat("d");
    return dateFormat.format(date)+dateFormat2.format(date)+getNumberSuffix(dateFormat2.format(date));
  }

  /** Formats the date in the form "MMM-d-YY" (Jan 6) */
  public static String formatDateMMMdsdYY (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MMM-d-yy");
    return dateFormat.format(date);
  }

  /** Formats the date in the form "MMM-dd-YYYY" (Jan-06-2008) */
  public static String formatDateMMMdDDdYYYY (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MMM-dd-yyyy");
    return dateFormat.format(date);
  }

  /** Formats the date in the form "EEE, MMMM d" (Tue, January 6) */
  public static String formatDateEEEcsMMMMsD (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("EEE, MMMM d");
    return dateFormat.format(date);
  }

  /** Formats the date in the form "EEE, MMMM d(suf)" (Tue, January 6) */
  public static String formatDateEEEcsMMMMsDu (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("EEE, MMMM ");
    SimpleDateFormat dateFormat2 = new SimpleDateFormat("d");
    return dateFormat.format(date)+dateFormat2.format(date)+getNumberSuffix(dateFormat2.format(date));
  }

  /** Formats the date in the form "EEE, MMM d(suf)" (Tue, Jan 6th) */
  public static String formatDateEEEcsMMMsDu (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("EEE, MMM ");
    SimpleDateFormat dateFormat2 = new SimpleDateFormat("d");
    return dateFormat.format(date)+dateFormat2.format(date)+getNumberSuffix(dateFormat2.format(date));
  }

  /** Formats the date in the form "EEEE, MMMM d" (Tuesday, January 6) */
  public static String formatDateEEEEcsMMMMsD (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("EEEE, MMMM d");
    return dateFormat.format(date);
  }

  /** Formats the date in the form "EEEE, MMMM d(suf)" (Tuesday, January 6) */
  public static String formatDateEEEEcsMMMMsDu (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("EEEE, MMMM ");
    SimpleDateFormat dateFormat2 = new SimpleDateFormat("d");
    return dateFormat.format(date)+dateFormat2.format(date)+getNumberSuffix(dateFormat2.format(date));
  }

  /** Formats the date in the form "EEE, MMMM d yyyy" (Tuesday, January 6 2004) */
  public static String formatDateEEEEcsMMMsDsYYYY (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("EEEE, MMMM d yyyy");
    return dateFormat.format(date);
  }

  /** Formats the date in the form "EEE, MMMM d(suf) yyyy" (Tuesday, January 6th 2004) */
  public static String formatDateEEEEcsMMMsDusYYYY (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("EEEE, MMMM ");
    SimpleDateFormat dateFormat2 = new SimpleDateFormat("d");
    SimpleDateFormat dateFormat3 = new SimpleDateFormat(" yyyy");
    return dateFormat.format(date)+dateFormat2.format(date)+getNumberSuffix(dateFormat2.format(date))+dateFormat3.format(date);
  }

  /** Formats the date (time) in the form H:MM AM */
  public static String formatDateHcMMsAM (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("h:mm a");
    return dateFormat.format(date);
  }

  /** Formats the date (time) in the form HH:MMAM */
  public static String formatDateHHcMMAM (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("hh:mma");
    return dateFormat.format(date);
  }
  
  /** Formats the date (time) in the form HH:MM */
  public static String formatDateHHcMM (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("HH:mm");
    return dateFormat.format(date);
  }

  /** Formats the date (time) in the form H:MMAM */
  public static String formatDateHcMMAM (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("h:mma");
    return dateFormat.format(date);
  }

  /** Formats the date (time) in the form (12:09pm on August 14th, 2007)*/
  public static String formatDateHHcMMAMsonsMMMMsDucsYYYY (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("HH:mma 'on' MMMM ");
    SimpleDateFormat dateFormat2 = new SimpleDateFormat("d");
    SimpleDateFormat dateFormat3 = new SimpleDateFormat(", yyyy");
    return dateFormat.format(date)+dateFormat2.format(date)+getNumberSuffix(dateFormat2.format(date))+dateFormat3.format(date);
  }

  /** Formats the date (time) in the form (August 14th, 2007 at 12:09 PM)*/
  public static String formatDateMMMMsDucsYYYYathcMMsAM (Date date) {
    SimpleDateFormat dateFormat1 = new SimpleDateFormat("MMMM ");
    SimpleDateFormat dateFormat2 = new SimpleDateFormat("d");
    SimpleDateFormat dateFormat3 = new SimpleDateFormat(", yyyy 'at' h:mm a");
    return dateFormat1.format(date)+dateFormat2.format(date)+getNumberSuffix(dateFormat2.format(date))+dateFormat3.format(date);
  }

  /** Formats the date (time) in the form "HAM EEE, MMM d(suf)" 3pm Mon, Jan 6th*/
  public static String formatDateHAMsEEEcsMMMsd (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("ha EEE, MMM ");
    SimpleDateFormat dateFormat2 = new SimpleDateFormat("d");
    return dateFormat.format(date)+dateFormat2.format(date)+getNumberSuffix(dateFormat2.format(date));
  }
   
  /** Formats the date (time) in the form MM/dd/yy H:MM AM */
  public static String formatDateMMsDDsYYsHcMMsAM (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MM/dd/yy h:mm a");
    return dateFormat.format(date);
  }

  /** Formats the date (time) in the form MMM dd(suf) yyyy H:MM AM */
  public static String formatDateMMMsDusYYYYsHcMMsAM (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MMMM ");
    SimpleDateFormat dateFormat2 = new SimpleDateFormat("d");
    SimpleDateFormat dateFormat3 = new SimpleDateFormat(" yyyy h:mm a");
    return dateFormat.format(date)+dateFormat2.format(date)+getNumberSuffix(dateFormat2.format(date))+dateFormat3.format(date);
  }
   
  public static String formatTimeHcmmAM(Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("h:mm a");
    return dateFormat.format(date);     
  }

  /** Formats the date (time) in the form MMM-dd-yy H:MM AM */
  public static String formatDateMMMdDDdYYsHcMMsAM (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MMM-dd-yy h:mm a");
    return dateFormat.format(date);
  }

  /** Formats the date (time) in the form MM/dd H:MMAM */
  public static String formatDateMMsDDsHcMMAM (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("MM/dd h:mma");
    return dateFormat.format(date);
  }

  /** Formats the date (time) in the form yyyy */
  public static String formatDateYY (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy");
    return dateFormat.format(date);
  }
   
  /** Formats the date (time) in the form HH */
  public static String formatDateHH (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("HH");
    return dateFormat.format(date);
  }
   
  /** Formats the date (time) in the form YYMMDD_HHMM */
  public static String formatDateYYMMDD_HHMM (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyMMdd_hhmm");
    return dateFormat.format(date);
  }
  
  /** Formats the date (time) in the form YYMMDD */
  public static String formatDateYYMMDD (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyMMdd");
    return dateFormat.format(date);
  }
  
  /** Formats the date (time) in the form yyMMdd_HHmm */
  public static String formatDateMilYYMMDD_HHMM (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyMMdd_HHmm");
    return dateFormat.format(date);
  }
   
  /** Formats the date (time) in the form YYYY-MM-DD/HH:MM:SS */
  public static String formatDateYYYYdMMdDDfsHHcMMcSS (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd/HH:mm:ss");
    return dateFormat.format(date);
  }
  
  public static String formatDateYYYYdMMdDDspHHcMMcSS (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
    return dateFormat.format(date);
  }
  
  /** Formats the date (time) in the form YYYY-MM-DD */
  public static String formatDateYYYYdMMdDD (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd");
    return dateFormat.format(date);
  }

  /** Formats the date (time) in the form YYYYMMDD */
  public static String formatDateYYYYMMDD (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyyMMdd");
    return dateFormat.format(date);
  }
  
  public static String formatDateYYYYMD (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyyMd");
    return dateFormat.format(date);
  }

  public static String formatDateYYYYfsMMfsDD (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd");
    return dateFormat.format(date);
  }
   
  /** Formats the date (time) in the form EEE H:MMAM */
  public static String formatDateEEEsHcMMAM (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("EEE h:mma");
    return dateFormat.format(date);
  }
  
  /** Returns the name of the day upon with the date occurs. */
  public static String getWeekdayName (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("EEEE");
    return dateFormat.format(date);
  }

  /** Returns the short name of the day upon with the date occurs. */
  public static String getWeekdayShortName (Date date) {
    SimpleDateFormat dateFormat = new SimpleDateFormat("EEE");
    return dateFormat.format(date);
  }

  public static String getMonthName (String month) {
    return getMonthName(Integer.parseInt(month));
  }

  public static String getMonthName (int month) {
    return months[month - 1];
  }
  
  public static Integer getMonthNumber (String month) {
    for(int i = 0; i < months.length; i++)
      if(month.toLowerCase().compareTo(months[i].toLowerCase()) == 0)
        return i+1;
    return null;
  }

  /** Returns the number with the spoken suffix addded. */
  public static String addNumberSuffix (int number) {
    return addNumberSuffix(String.valueOf(number));
  }

  /** Returns the number with the spoken suffix addded. */
  public static String addNumberSuffix (String number) {
    return number + getNumberSuffix(number);
  }

  /** Returns the spoken suffix that can be added to the specified number. */
  public static String getNumberSuffix (int number) {
    return getNumberSuffix(String.valueOf(number));
  }

  /** Returns the spoken suffix that can be added to the specified number. */
  public static String getNumberSuffix (String number) {
    // Based on code by Marc Wallace (http://www.wallace.net/marc/)

    int length = number.length();
    if (length > 1) {
      char secondToLastDigit = number.charAt(length - 2);
      if (secondToLastDigit == '1') // Numbers in the teens (12th, 316th, 3214th)
        return "th";
    }

    char lastDigit = (length > 0) ? number.charAt(length - 1) : '0'; 
    switch (lastDigit) {
      case '1':  return "st"; // Non-teen numbers ending in 1 (1st, 51st, 971st)
      case '2':  return "nd"; // Non-teen numbers ending in 2 (2nd, 72nd, 1642nd)
      case '3':  return "rd"; // Non-teen numbers ending in 3 (3rd, 23rd, 933rd)
      default:   return "th"; // All other numbers (8th, 64th, 627th)
    }
  }

  /** Retrieves the stack trace as a human readable string. */
  public static String getPrintableStackTrace (Throwable throwable) {
    StringWriter sw = new StringWriter();
    PrintWriter pw = new PrintWriter(sw);
    throwable.printStackTrace(pw);
    pw.flush();
    return sw.toString();
  }

  /**
   * Builds a string from the template resource (@see fillInTemplate).  If the
   * specified resource is not found or fails to load, an exception is thrown.
   */
  public static String fillInTemplateResource (String templateURL, Map<String,String> paramMap) throws Exception {
    String template = loadStringResource(templateURL);
    return fillInTemplate(template, paramMap);
  }

  /**
   * Builds a string from the template, filling template variables from the
   * supplied param map.  Template parameters appear in the form
   * &lt;&lt;PARAMNAME&gt;&gt; or &lt;&lt;PARAMNAME|DEFAULTVALUE&gt;&gt;
   * within the template string.  The PARAMNAME is used as a key to lookup
   * the value from the param map.  If no entry is found the default value
   * is used (or the empty string if not default value is supplied).
   */
  public static String fillInTemplate (String template, Map<String,String> paramMap) {
    Pattern pattern = Pattern.compile("<<(.*?)(\\|(.*?))?>>(\\{\\{(.*?)\\}\\})?", Pattern.DOTALL);
    StringBuffer resultBuffer = new StringBuffer(template.length());
    fillInTemplateRecurse(pattern, template, paramMap, resultBuffer);
    return resultBuffer.toString();
  }

  private static void fillInTemplateRecurse (Pattern pattern, String template, Map<String,String> paramMap, StringBuffer resultBuffer) {
    Matcher matcher = pattern.matcher(template);

    int textStart = 0;
    while (matcher.find()) {
      int textEnd = matcher.start();
      if (textEnd - textStart > 0) {
        resultBuffer.append(template.substring(textStart, textEnd));
      }

      String paramName = matcher.group(1);
      String paramValue = (String) paramMap.get(paramName);
      if (paramValue == null) {
        String defaultValue = matcher.group(3);
        paramValue = defaultValue;
      }

      if (paramValue != null) {
        String segment = matcher.group(5);
        if (segment != null) {
          Map<String,String> subParamMap = new HashMap<String,String>(paramMap);
          subParamMap.put(paramName, paramValue);

          fillInTemplateRecurse(pattern, segment, subParamMap, resultBuffer);
        }
        else {
          resultBuffer.append(paramValue);
        }
      }

      textStart = matcher.end();
    }
    resultBuffer.append(template.substring(textStart));
  }

  /** Removes any non-ISBN characters (including dashes) from the provided ISBN. */
  public static String cleanISBN (String isbn) {
    return isbn.toUpperCase().replaceAll("[^\\dX]", ""); // Remove everything but digits
  }

  public static String escapeJavaScriptString (String text) {
    return text.replaceAll("\"", "\"\"");
  }

  /** Generates a Random ID using upper- and lower-case letters and digits. */
  public static String createRandomID (int length) {
    if (defaultRandomIDCharacterSet == null) {
      defaultRandomIDCharacterSet = new char[26 + 26 + 10];
      int i = 0;
      for (char ch = '0'; ch <= '9'; ch++)
        defaultRandomIDCharacterSet[i++] = ch;
      for (char ch = 'A'; ch <= 'Z'; ch++)
        defaultRandomIDCharacterSet[i++] = ch;
      for (char ch = 'a'; ch <= 'z'; ch++)
        defaultRandomIDCharacterSet[i++] = ch;
    }

    return createRandomID(length, defaultRandomIDCharacterSet);
  }

  /** Generates a Random ID using upper- and lower-case letters and digits. */
  public static String createRandomNumericID (int length) {
    if (numericRandomIDCharacterSet == null) {
      numericRandomIDCharacterSet = new char[10];
      int i = 0;
      for (char ch = '0'; ch <= '9'; ch++)
        numericRandomIDCharacterSet[i++] = ch;
    }

    return createRandomID(length, numericRandomIDCharacterSet);
  }

  /** Generates a Random ID consiting of characters from the specified set. */
  public static String createRandomID (int length, char[] charSet) {
    SecureRandom random = new SecureRandom();

    StringBuffer id = new StringBuffer();
    while (id.length() < length) {
      int index = random.nextInt(charSet.length);
      char ch = charSet[index];
      id.append(ch);
    }

    return id.toString();
  }

  /** Performs a deep copy using serialization (slow). */
  public static Object serializationDeepCopy (Object object) throws Exception {
    ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
    ObjectOutputStream objectOutputStream = new ObjectOutputStream(byteArrayOutputStream);
    objectOutputStream.writeObject(object);
    objectOutputStream.close();
    byte[] serializedData = byteArrayOutputStream.toByteArray();
    ByteArrayInputStream byteArrayInputStream = new ByteArrayInputStream(serializedData);
    ObjectInputStream objectInputStream = new ObjectInputStream(byteArrayInputStream);
    return objectInputStream.readObject();
  }

  /** Checks if an email address is structural valid. */
  public static boolean isValidEmailAddress (String emailAddress) {
    return Pattern.matches("[^\\@\\s]+\\@[^\\@\\s\\.]+\\.[^\\@\\s]+", emailAddress);
  }
  
  /** takes a price and returns it as an integer amount if valid otherwise -1 */ 
  public static int UnPrint(String Price)
  {
    int x = 0;
    String Temp = Price;
    if(Temp.indexOf(".") == -1)
    {
      if(Temp.length() == 1 && Temp.charAt(0) == '0')
        return 0;
      return -1;
    }
    if(Temp.length() <= 3)
      return -1;
    if(Temp.indexOf(".") > Temp.length()-2)
      return -1;
    if(Price.charAt(0) == '$')
      Temp = Price.substring(1);
    Temp = Temp.substring(0,Temp.indexOf(".")) + Temp.substring(Temp.indexOf(".")+1,Temp.indexOf(".")+3);
    for(int i = 0; i < Temp.length(); i++)
    {
      if(Temp.charAt(i) < '0' || Temp.charAt(i) > '9')
      {
        return -1;
      }
    }
    x = Integer.parseInt(Temp);
    return x;
  }

  /** Builds a list containing the single object specified. */
  public static List makeSingleObjectList (Object object) {
    List<Object> list = new LinkedList<Object>();
    list.add(object);
    return list;
  }
  
  public static <Type> Set<Type> makeSingleTypedSet (Type object) {
    Set<Type> set = new HashSet<Type>();
    set.add(object);
    return set;
  }

  /** Builds a list containing the single typed object specified. */
  public static <Type> List<Type> makeSingleTypedList (Type object) {
    List<Type> list = new LinkedList<Type>();
    list.add(object);
    return list;
  }

  public static <ElementType> ElementType[] mergeArrays (Class<ElementType[]> arrayClass, ElementType[] array1, ElementType[] array2) {
    ElementType[] mergedArray = arrayClass.cast(Array.newInstance(arrayClass.getComponentType(), array1.length + array2.length));
    System.arraycopy(array1, 0, mergedArray, 0, array1.length);
    System.arraycopy(array2, 0, mergedArray, array1.length, array2.length);
    return mergedArray;
  }

  /**
   * Splits the name into first and last portions.
   * 
   * @param fullName The full name.
   * 
   * @return A String[2] array with index 0 containing the first name and index 1 containing the last name.
   */
  public static String[] splitNameIntoFirstLast (String fullName) {
    int spaceIndex = fullName.lastIndexOf(' ');
    String firstName = (spaceIndex >= 0) ? fullName.substring(0, spaceIndex).trim() : fullName.trim();
    String lastName = (spaceIndex >= 0) ? fullName.substring(spaceIndex).trim() : "";
    return new String[] { firstName, lastName };
  }

  public static BigDecimal findPercentSaved  (BigDecimal ourPrice, BigDecimal listPrice) {
    BigDecimal decimal100_00 = new BigDecimal("100.00");
    BigDecimal priceToListPriceRatio = ourPrice.multiply(decimal100_00).divide(listPrice,BigDecimal.ROUND_HALF_UP);
    BigDecimal percentOffListPrice = decimal100_00.subtract(priceToListPriceRatio);
    return percentOffListPrice.setScale(0, BigDecimal.ROUND_DOWN);
  }

  public static Integer calcMassagedBuyerDiscount (int listPrice, int marketPrice) {
    if (listPrice < 100)
      return null;

    int discount = listPrice - marketPrice;
    if (discount < 0)
      return null;

    return new Integer(discount);
  }

  public static Integer calcMassagedBuyerDiscountPercentage (int listPrice, int marketPrice) {
    Integer discount = calcMassagedBuyerDiscount(listPrice, marketPrice);
    if (discount == null)
      return null;

    int discountPercent = (discount.intValue() * 100) / listPrice;
    return new Integer(discountPercent);
  }

  //Used to create a UniqueID, not for the request feedback email
  public static String getUniqueID(String firstName, String lastName)
  {
    int total=0;
    int lastNameTotal = 0;
    for(int i=0;i<lastName.length();i++)
    {
      lastNameTotal+=(int)lastName.charAt(i);
    }                 
    int firstNameTotal = 0;
    for(int i=0;i<firstName.length();i++)
    {
      firstNameTotal+=(int)firstName.charAt(i);
    }   
    total=Math.abs(lastNameTotal-firstNameTotal);
    total = total + 10000;
    total = total ^ 8;
    total = total ^ 256;
    total = total ^ 1024;
    return String.valueOf(total);
  }

  public static BufferedInputStream toBufferedInputStream (InputStream inputStream) {
    return (inputStream instanceof BufferedInputStream)
      ? (BufferedInputStream) inputStream
      : new BufferedInputStream(inputStream);
  }

  public static BufferedOutputStream toBufferedOutputStream (OutputStream outputStream) {
    return (outputStream instanceof BufferedOutputStream)
      ? (BufferedOutputStream) outputStream
      : new BufferedOutputStream(outputStream);
  }

  /** An compare method that handles either object being null. */
  public static <Type extends Comparable<Type>> int compare (Type object1, Type object2, boolean nullIsLessThan) {
    if (object1 == null) {
      if (object2 == null) {
        return 0;
      }
      else {
        return nullIsLessThan ? -1 : +1;
      }
    }
    else {
      if (object2 == null) {
        return nullIsLessThan ? +1 : -1;
      }
      else {
        return object1.compareTo(object2);
      }
    }
  }

  /** An equals comparison that handles either object being null. */
  public static boolean equals (Object object1, Object object2) {
    return (object1 == null)
      ? object2 == null
      : object1.equals(object2);
  }

  /** A hashcode producer that handles null objects. */
  public static int hashCode (Object object) {
    return (object != null) ? object.hashCode() : 0;
  }

  public static int[] getUniqueImageIndex(int size)
  {
    Random generator = new Random();
    int index[];
    if(size==3)
    {
      index = new int[3];
    }
    else if(size==4)
    {
      index = new int[4];
    }
    else
    {
      index = new int[5];      
    }
    int imageIndex = generator.nextInt(size);
    index[0] = imageIndex;
    boolean notOkay=true;
    for(int i=1;i<index.length;i++)
    {
      notOkay = true;
      while(notOkay)
      {
        notOkay = false;
        imageIndex = generator.nextInt(size);
        for(int j=0;j<i;j++)
        {
          if(imageIndex==index[j])
          notOkay = true;
        }
      }
      index[i] = imageIndex;
    }
    return index;
  }

  public static String intToDayOfWeek(int dayOfWeek) {
    String weekdayName = null;
    switch(dayOfWeek) {
      case 0:
        weekdayName = "Sun";
        break;
      case 1:
        weekdayName = "Mon";        
        break;
      case 2:
        weekdayName = "Tue";        
        break;
      case 3:
        weekdayName = "Wed";        
        break;
      case 4:
        weekdayName = "Thu";        
        break;
      case 5:
        weekdayName = "Fri";
        break;
      default:
        weekdayName = "Sat";
    }
    return weekdayName;
  }

  public static String formatDaysHoursMinutesSeconds (int timeSpanInSeconds) {
    String description = "";
    String prefix = "";

    int days = timeSpanInSeconds / (24 * 60 * 60);
    if (days != 0) {
      description = days + " days";
      prefix = " ";
      timeSpanInSeconds -= days * (24 * 60 * 60);
    }

    int hours = timeSpanInSeconds / (60 * 60);
    if (hours != 0) {
      description = prefix + hours + " hours";
      prefix = " ";
      timeSpanInSeconds -= hours * (60 * 60);
    }

    int minutes = timeSpanInSeconds / 60;
    if (minutes != 0) {
      description = prefix + minutes + " minutes";
      prefix = " ";
      timeSpanInSeconds -= minutes * 60;
    }

    int seconds = timeSpanInSeconds;
    if (seconds != 0 || description.length() == 0) {
      description = prefix + seconds + " seconds";
    }

    return description;
  }

  /**
   * Returns the broken-out components of the provided IP address.  If a non-integer
   * component is found (or there are not enough components), then the rest of the
   * components will be null.  This can be useful for parsing IP specs with wildcard
   * components (ex: "141.238.*" will retun Short[] { 141, 238, null, null }). 
   */
  public static Short[] parseIPAddress (String ipAddress, boolean returnNullWhenNoComponents) {

    Short[] components = new Short[4];

    if (ipAddress != null) {
      String[] componentStrings = ipAddress.split("\\.");
      int numComponents = Math.min(componentStrings.length, 4);
      try {
        for (int i = 0; i < numComponents; i++) {
          components[i] = new Short(componentStrings[i]);
        }
      }
      catch (NumberFormatException nfe) {
        // Ignore - leave any other components unparsed
      }
    }

    if (returnNullWhenNoComponents && components[0] == null)
      return null;

    return components;
  }
  
  /**
   * Returns a color object from a html hex color value
   */
  public static Color getColorFromHEX(String value) {
    if (value != null && value.length() == 6) {
      return Color.decode(String.valueOf(Integer.parseInt(value,16)));
    }
    else {
      return Color.WHITE;
    }
  }

  /**
   * Returns a hex code from a color object
   */
  public static String getHEXFromColor(Color color) {
    return Integer.toHexString(color.getRGB());    
  }

  @SuppressWarnings("unchecked")
  public static <Type> Type[] toArray (List<Type> list, Class<Type> typeClass) {

    Type[] array = (Type[]) Array.newInstance(typeClass, list.size());
    list.toArray(array);
    return array;
  }
  
  public static <Type> List<List<Type>> createOrderedColumnTable(List<Type> dataList, final int numberOfColumns) {

    List<List<Type>> table = new ArrayList<List<Type>>();
    int entriesPerColumn = (dataList.size() / numberOfColumns) + 1;
    
    int rowNumber = 0;
    int column = 0;
    List<Type> row = null;
    for(Type dataObject : dataList) {
      
      if(rowNumber >= entriesPerColumn) {
        
        rowNumber = 0;
        ++column;
      }
      
      if(column == 0) {
        
        row = new ArrayList<Type>();
        table.add(row);
      }
      else {
        row = table.get(rowNumber);
      }
      
      row.add(dataObject);
      
      ++rowNumber;
    }
    
    return table;
  }
  
  public static String properName(String name) {
    
    return name.substring(0, 1).toUpperCase() + name.substring(1).toLowerCase();
  }
  
  public static void log(String message) {
    
    System.out.println("[" + new Date() + "] " + message);
  }
  
  public static <ReturnType, PassingType> Map<ReturnType, PassingType> map(Collection<PassingType> collection, GenericAccessor<PassingType, ReturnType> genericAccessor) {
    
    Map<ReturnType, PassingType> map = new HashMap<ReturnType, PassingType>();
    
    for(PassingType passingType : collection) {
      
      map.put(genericAccessor.get(passingType), passingType);
    }
    
    return map;
  }
}