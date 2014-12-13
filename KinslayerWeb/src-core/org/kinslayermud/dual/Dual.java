package org.kinslayermud.dual;

public class Dual<T1, T2> {

  T1 object1;
  T2 object2;
  
  public Dual(T1 object1, T2 object2) {
    
    setObject1(object1);
    setObject2(object2);
  }
  
  public T1 getObject1() {
    
    return object1;
  }
  
  public void setObject1(T1 object1) {
    
    this.object1 = object1;
  }

  public T2 getObject2() {
    
    return object2;
  }
  
  public void setObject2(T2 object2) {
    
    this.object2 = object2;
  }
}
