package org.kinslayermud.util;

import java.math.BigDecimal;
import java.text.SimpleDateFormat;
import java.util.Collection;
import java.util.EnumSet;
import java.util.Iterator;
import java.util.List;

import org.kinslayermud.enumerator.VEnum;

/** SQL / JDBC utility methods. */
public abstract class SQLUtil {
  
  /** SQL representing the true boolean-int value. */
  public static final String TRUE_BOOLINTSQL = "1";

  /** SQL representing the false boolean-int value. */
  public static final String FALSE_BOOLINTSQL = "0";

  /** Escapes a string for use in an SQL statement. */
  public static String escapeString (String text) {
    return text.replaceAll("'", "''").replaceAll("\\\\", "\\\\\\\\");
  }

  /** 
   * Escapes and quotes a string for use in an SQL statement.
   * If the string is null, "NULL" is returned instead.
   */
  public static String escapeQuoteString (String text) {
    return (text != null) ? "'" + escapeString(text) + "'" : "NULL";
  }

  /** Encodes and quotes the timestamp for use in an SQL statement. */
  public static String encodeQuoteTimestamp (java.util.Date timestamp) {
    return encodeQuoteDate(timestamp);
  }

  /** Encodes and quotes the timestamp for use in an SQL statement. */
  public static String encodeQuoteDate (java.util.Date timestamp) {
    return (timestamp != null)
                ? new SimpleDateFormat("''yyyyMMddHHmmss''").format(timestamp)
                : "NULL";
  }
  
  /** Encodes and quotes the timestamp for use in an SQL statement. */
  public static String encodeQuoteTime (java.util.Date timestamp) {
    return (timestamp != null)
                ? new SimpleDateFormat("''HH:mm:ss''").format(timestamp)
                : "NULL";
  }

  /** Encodes and quotes the day portion only of a date for use in an SQL statement. */
  public static String encodeQuoteDate_DayPortionOnly (java.util.Date timestamp) {
    return (timestamp != null)
                ? new SimpleDateFormat("''yyyyMMdd''").format(timestamp)
                : "NULL";
  }

  /** Encodes a boolean value as an SQL integer value. */
  public static String encodeBooleanInt (boolean value) {
    return value ? TRUE_BOOLINTSQL : FALSE_BOOLINTSQL;
  }

  /**
   * Encodes a boolean value as an SQL integer value.
   * If value is null then NULL is returned.
   */
  public static String encodeBooleanInt (Boolean value) {
    if (value == null)
      return "NULL";

    return encodeBooleanInt(value.booleanValue());
  }
  
  public static String putFixedIntegerBigDecimal (BigDecimal value) {
    if (value == null)
      return "NULL";

    return value.movePointRight(2).toString();
  }

  /** Encodes and quotes the enum for use in an SQL statement. */
  public static String encodeQuoteEnum (Enum value) {
    return (value != null)
             ? "'" + value + "'"
             : "NULL";
  }

  /** Encodes the venum for use in an SQL statement. */
  public static String encodeVEnum (VEnum vEnum) {
    return (vEnum != null)
             ? String.valueOf(vEnum.value())
             : "NULL";
  }

  /**
   * Builds an SQL expression representing the specified date criteria.
   * 
   * @param timeSpan The time span to be represented by the date criteria.
   * 
   * @return SQL representing the date criteria.
   */
  public static String getTimeSpanCriteriaSQL (String dateField, TimeSpan timeSpan) {

    String startDataCriteria = (timeSpan != null && timeSpan.startTime != null)
                                  ? encodeQuoteTimestamp(timeSpan.startTime) + " <= " + dateField
                                  : null;
    String endDataCriteria = (timeSpan != null && timeSpan.endTime != null)
                                ? dateField + " < " + encodeQuoteTimestamp(timeSpan.endTime)
                                : null;

    if (startDataCriteria != null) {
      if (endDataCriteria != null) {
        return "(" + startDataCriteria + " AND " + endDataCriteria + ")";
      }
      else {
        return startDataCriteria;
      }
    }
    else {
      if (endDataCriteria != null) {
        return endDataCriteria;
      }
      else {
        return "1 = 1";
      }
    }
  }

  /**
   * Builds an SQL expression representing date-criteria that limits selection 
   * entries with dates occurring today
   */
  public static String getTodayTimeSpanCriteriaSQL (String dateField) {
    TimeSpan timeSpan = TimeSpan.getTimeSpanForToday();
    return getTimeSpanCriteriaSQL(dateField, timeSpan);
  }

  /**
   * Builds an SQL expression representing date-criteria that limits selection 
   * entries with dates occuring between now and the end of today.
   */
  public static String getRestOfTodayTimeSpanCriteriaSQL (String dateField) {
    TimeSpan timeSpan = TimeSpan.getTimeSpanForRestOfToday();
    return getTimeSpanCriteriaSQL(dateField, timeSpan);
  }

  /**
   * Creates an SQL list from the items in the provided list.
   * 
   * @param set The source item collection.
   * @param quoteElements Whether the items should be enclosed in single quotes.
   * @param onEmptyAddNull If true and the source list is empty, a list containing only null will be returned. 
   * 
   * @return An SQL list representing the source item list. 
   */
  public static String buildListSQL (Collection set, boolean quoteElements, boolean onEmptyAddNull) {
    if (onEmptyAddNull && set.isEmpty())
      return "(null)";

    StringBuilder strBuf = new StringBuilder("(");
    Iterator iter = set.iterator();
    boolean firstPass = true;
    while (iter.hasNext()) {
      if (firstPass)
        firstPass = false;
      else
        strBuf.append(',');

      Object element = iter.next();
      if (element == null)
        strBuf.append("NULL");
      else if (quoteElements)
        strBuf.append(escapeQuoteString(element.toString()));
      else
        strBuf.append(element);
    }
    strBuf.append(')');
      return strBuf.toString();
  }

  /**
   * Builds SQL WHERE-criteria for the specified field based on the provided IP
   * address spec.  The IP address spec can be a full IP address or one set with
   * a component replaced by '*' (ex: "141.238.30.*"; ex: "141.*").
   */
  public static String buildIPAddressCriteria (String ipField, String ipMatchSpec) {

    Short[] ipComponents = MiscUtil.parseIPAddress(ipMatchSpec, true);

    if (ipComponents == null)
      return "1 = 1";

    String ipValue = "";
    boolean exactMatch = true;
    for (int i = 0; i < ipComponents.length; i++) {
      if (i != 0) {
        ipValue += '.';
      }
      if (ipComponents[i] == null) {
        ipValue += '%';
        exactMatch = false;
        break;
      }
      ipValue += ipComponents[i];
    }

    return exactMatch
             ? ipField + " = " + SQLUtil.escapeQuoteString(ipValue)
             : ipField + " LIKE " + SQLUtil.escapeQuoteString(ipValue);
  }

  /**
   * Builds a column spec (for using in a CREATE/ALTER TABLE statements).
   * 
   * @param enumClass The enum class for which to create the spec.
   * 
   * @return A enum column type spec.
   */
  public static String buildEnumFieldSpec (Class<? extends Enum> enumClass) {

    @SuppressWarnings("unchecked") EnumSet<? extends Enum> enumValueSet = EnumSet.allOf(enumClass);

    StringBuilder enumSetSpec = new StringBuilder("ENUM (");

    boolean firstValue = true;
    for (Enum value : enumValueSet) {

      if (firstValue)
        firstValue = false;
      else
        enumSetSpec.append(',');

      enumSetSpec.append('\'');
      enumSetSpec.append(value);
      enumSetSpec.append('\'');
    }

    enumSetSpec.append(')');
    
    return enumSetSpec.toString();
  }
  
  public static String buildStringFromList (List<String> values) {
    return StringUtil.buildStringFromList(values, StringUtil.MYSQL_STORAGE_SEPERATOR_SEQUENCE);
  }
  
  public static String buildLimit(Integer offset, Integer fetchSize) {
    
    if(offset == null && fetchSize == null) {
      return "";
    }
    
    StringBuilder stringBuilder = new StringBuilder(" LIMIT ");
    
    if(offset != null && fetchSize != null) {
      
      stringBuilder.append(offset + "," + fetchSize);
    }
    else if(offset != null) {
      
      stringBuilder.append(offset);
    }
    else {
      
      stringBuilder.append(fetchSize);
    }
    
    return stringBuilder.toString();
  }
}