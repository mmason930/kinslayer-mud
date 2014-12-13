package org.kinslayermud.genericaccessor;

public interface GenericAccessor <PassingType, ReturnType> {

  ReturnType get(PassingType passingType);
}
