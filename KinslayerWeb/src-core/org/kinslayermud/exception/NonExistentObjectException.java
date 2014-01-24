package org.kinslayermud.exception;

public class NonExistentObjectException extends Exception {

  public NonExistentObjectException(String reason) {
    super(reason);
  }
}

