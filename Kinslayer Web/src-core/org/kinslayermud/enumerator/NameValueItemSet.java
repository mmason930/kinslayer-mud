package org.kinslayermud.enumerator;

import java.util.ArrayList;
import java.util.List;

/**
 * Utility class that helps associate name value pairs as singular /
 * unique items.  Useful for things like selections in combo-boxes or
 * basic mapping between names, values, and items.  This class manages
 * the name-value associations and provides methods for converting
 * between items, names, and values.
 */
public class NameValueItemSet implements Cloneable {

  /** List of NameValue associations == list of items. */
  protected List<Object> itemList = new ArrayList<Object>();

  ////////////////////////////////////////////////////////////////////////////////

  /** Default constructor.  Sets up an initially empty list. */
  public NameValueItemSet () { }

  /**
   * Constructor.  Creates items from the specified name-value pair array.
   * See {@link #addItems(Object[][])} for info on the array layout. 
   */
  public NameValueItemSet (Object[][] nameValueList) {
    addItems(nameValueList);
  }

  /**
   * Takes an array of name-value pair arrays and adds each pair as an item.
   * For example:<br>
   * <br>
   * <pre>
   *    Object[][] nameValueList = new Object[][] {
   *       { "Choose One", null },
   *       { "Item One",   object1 },
   *       { "Item Two",   object2 },    
   *    };
   *    new NameValueItemSet(nameValueList);
   * </pre> 
   * <br>
   * NOTE: Name elements do not have to be String.  The toString method of the
   * name object is used to obtain the display text for the pair.<br>
   * NOTE: null can be used as a value (but not a name).
   */
  public synchronized void addItems (Object[][] nameValueList) {
    for (int i = 0; i < nameValueList.length; i++) {
      Object[] nameValueSet = nameValueList[i];
      addItem((String)nameValueSet[0], nameValueSet[1]);
    }
  }

  /** Adds the items from the specified set into this set. */
  public synchronized void addItems (NameValueItemSet fromSet) {
    int numItems = fromSet.itemList.size();
    for (int i = 0; i < numItems; i++) {
      Object item = fromSet.itemList.get(i);
      itemList.add(item);
    }
  }

  /**
   * Add a name-value association as an item.
   * 
   * @return The added item.
   */
  public synchronized Object addItem (String name, Object value) {
    NameValue item = new NameValue(name, value);
    itemList.add(item);
    return item;
  }

  /**
   * Remove an item.
   * 
   * @param item The item to remove.
   */
  public synchronized void removeItem (Object item) {
    itemList.remove(item);
  }

  /**
   * Remove an item.<br>
   * <br>
   * Note: Only the first item found matching the value will be removed.
   * 
   * @param value The value of the item.
   */
  public synchronized void removeItemByValue (Object value) {
    Object item = getItemForValue(value);
    removeItem(item);
  }

  public synchronized List getItemList () {
    return new ArrayList<Object>(itemList);
  }

  /**
   * Convert the value into the uniqule identified item.  Useful for things like
   * combo-box selections.  For example, to select the item associated with a given
   * value, do the following:<br>
   * <br>
   * <pre>
   *    Object valueToSelect = {some programmatic value};
   *    Object comboBoxItem = NameValueItemSet.getItemForValue(valueToSelect);
   *    comboBox.setSelectedItem(comboBoxItem);
   * </pre>
   */
  public synchronized Object getItemForValue (Object value) {
    int numItems = itemList.size();
    for (int i = 0; i < numItems; i++) {
      NameValue item = (NameValue) itemList.get(i);
      if (value == null) {
        if (item.value == null)
          return item;
      }
      else if (value.equals(item.value)) {
        return item;
      }
    }
    return null;
  }

  /**
   * Retrieve the value associated with the unique item.  For example, to retrieve
   * the value associated with the selected item in a ComboBox, do the following<br>
   * <br>
   * <pre>
   *    Object selectedComboBoxItem = comboBox.getSelectedItem();
   *    Object selectedValue = NameValueItemSet.getValueForItem(selectedComboBoxItem);
   * </pre>
   */
  public synchronized Object getValueForItem (Object item) {
    if (item == null)
      return null;

    NameValue nameValue = (NameValue) item;
    return nameValue.value;
  }
  
  /**
   * Retrieve the name associated with the unique item.
  */
  public synchronized String getNameForItem (Object item) {
    if (item == null)
      return null;

    NameValue nameValue = (NameValue) item;
    return nameValue.name;
  }

  /** Retrieves the name associated with the specified value. */
  public synchronized Object getNameForValue (Object value) {
    NameValue item = (NameValue) getItemForValue(value);
    return (item != null) ? item.name : null;
  }

  // @see java.lang.Object#clone()
  public Object clone () {
    NameValueItemSet nameValueItemSet = new NameValueItemSet();
    nameValueItemSet.itemList = new ArrayList<Object>(itemList);
    return nameValueItemSet;
  }
}