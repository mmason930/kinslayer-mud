package org.kinslayermud.enumerator;

import java.io.Serializable;

import org.kinslayermud.util.MiscUtil;


public class NameValue implements Serializable {

  private static final long serialVersionUID = 1L;

  public String name;
  public Object value;
 
  public NameValue () {
    init(null, null);
  }
  
  public NameValue (String name, Object value) {
    init(name, value);
  }
  
  public NameValue (NameValue nameValue) {
    init(nameValue.name, nameValue.value);
  }
  
  private void init(String name, Object value) {
    this.value = value;
    this.name = name;
  }

  public String getName() {
    return name;
  }

  public String toString () {
    return name;
  }

  public boolean equals(Object compareObject) {
    if (!(compareObject instanceof NameValue))
      return false;

    NameValue compareNameValue = (NameValue) compareObject;
    return MiscUtil.equals(name, compareNameValue.name)
           && MiscUtil.equals(value, compareNameValue.value);
  }

  public int hashCode() {
    return (name != null)
             ? name.hashCode()
             : 0;
  }
}