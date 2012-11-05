package org.kinslayermud.util;

import org.kinslayermud.character.User;
import org.kinslayermud.exception.DataInterfaceException;


public interface WebSupport {
  
  public User performLogin(String username, String password) throws DataInterfaceException;
  public void createUserSession(int userId, String sessionId) throws DataInterfaceException;
  public User getUserFromSession(String sessionId) throws DataInterfaceException;
}
