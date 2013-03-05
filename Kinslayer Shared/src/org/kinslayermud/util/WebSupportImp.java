package org.kinslayermud.util;

import java.sql.Connection;
import java.sql.Statement;
import java.util.Collection;
import java.util.List;
import java.util.Map;

import org.kinslayermud.advertising.FeaturedMUDListing;
import org.kinslayermud.advertising.FeaturedMUDListingUtil;
import org.kinslayermud.character.User;
import org.kinslayermud.comm.Comm;
import org.kinslayermud.comm.CommUtil;
import org.kinslayermud.exception.DataInterfaceException;
import org.kinslayermud.help.HelpFile;
import org.kinslayermud.help.HelpUtil;
import org.kinslayermud.kit.KitUtil;
import org.kinslayermud.kit.KitWithItemsAndObjectPrototypes;
import org.kinslayermud.misc.Provider;
import org.kinslayermud.mob.MobPrototype;
import org.kinslayermud.mob.MobUtil;
import org.kinslayermud.mob.SuperMob;
import org.kinslayermud.object.ObjectPrototype;
import org.kinslayermud.object.ObjectUtil;
import org.kinslayermud.object.ObjectWeaponType;
import org.kinslayermud.object.ObjectWearType;
import org.kinslayermud.playerkilling.PKUtil;
import org.kinslayermud.playerkilling.PlayerKill;
import org.kinslayermud.user.login.UserLogin;
import org.kinslayermud.user.login.UserLoginUtil;
import org.kinslayermud.userlog.UserLog;
import org.kinslayermud.userlog.UserLogRecord;
import org.kinslayermud.userlog.UserLogUtil;
import org.kinslayermud.zone.Zone;
import org.kinslayermud.zone.ZoneUtil;


public class WebSupportImp implements WebSupport {

  protected Provider provider;
  public WebSupportImp(Provider provider) {
    this.provider = provider;
  }
  
  public User performLogin(String username, String password) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      User user = UserUtil.performLogin(statement, username, password);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return user;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public void createUserSession(int userId, String sessionId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      UserUtil.createUserSession(statement, userId, sessionId);
      
      statement.close();
      connection.commit();
      connection.close();
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public User getUserFromSession(String sessionId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      User user = UserUtil.getUserFromSession(statement, sessionId);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return user;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public void performPlayerPortalSignOut(String sessionId, int userId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      UserUtil.performPlayerPortalSignOut(statement, sessionId, userId);
      
      statement.close();
      connection.commit();
      connection.close();
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  

  public List<UserLogRecord> getUserLogRecords(int userId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<UserLogRecord> userLogRecords = UserLogUtil.getUserLogRecords(statement, userId);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return userLogRecords;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public UserLog getUserLog(int userLogId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      UserLog userLog = UserLogUtil.getUserLog(statement, userLogId, false);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return userLog;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public String getInstanceDomain() {
    
    return provider.getInstanceDomain();
  }

  public MobPrototype getMobPrototype(int mobPrototypeId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      MobPrototype mobPrototype = MobUtil.getMobPrototype(statement, mobPrototypeId, false);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return mobPrototype;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public Map<Integer, MobPrototype> getMobPrototypeMap(Collection<Integer> mobPrototypeIdCollection) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      Map<Integer, MobPrototype> mobPrototypeMap = MobUtil.getMobPrototypeMap(statement, mobPrototypeIdCollection);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return mobPrototypeMap;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public Zone getZoneLoadingSuperMobPrototype(int mobPrototypeId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      Zone zone = ZoneUtil.getZoneLoadingSuperMobPrototype(statement, mobPrototypeId, false);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return zone;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public KitWithItemsAndObjectPrototypes getKitWithItemsAndObjectPrototypes(int kitId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      KitWithItemsAndObjectPrototypes kitWithItemsAndObjectPrototypes = KitUtil.getKitWithItemsAndObjectPrototypes(statement, kitId, false);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return kitWithItemsAndObjectPrototypes;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public Map<Integer, User> getUserMap(Collection<Integer> userIdCollection) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      Map<Integer, User> userMap = UserUtil.getUserMap(statement, userIdCollection);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return userMap;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  public List<PlayerKill> getLastSoManyPlayerKills(int howMany) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<PlayerKill> playerKills = PKUtil.getLastSoManyPlayerKills(statement, howMany);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return playerKills;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public List<SuperMob> getAllOpenSuperMobs() throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<SuperMob> superMobs = MobUtil.getAllOpenSuperMobs(statement);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return superMobs;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public List<ObjectPrototype> getObjectPrototypesByWearType(ObjectWearType objectWearType) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<ObjectPrototype> superMobs = ObjectUtil.getObjectPrototypesByWearType(statement, objectWearType);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return superMobs;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }

  public List<ObjectPrototype> getObjectPrototypesByWeaponType(ObjectWeaponType objectWeaponType) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<ObjectPrototype> superMobs = ObjectUtil.getObjectPrototypesByWeaponType(statement, objectWeaponType);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return superMobs;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public List<PlayerKill> getPlayerKillsByKillerId(int killerUserId) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<PlayerKill> playerKills = PKUtil.getPlayerKillsByKillerId(statement, killerUserId);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return playerKills;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public List<UserLogin> getUserLoginsByUserId(int userId) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<UserLogin> userLogins = UserLoginUtil.getUserLoginsByUserId(statement, userId);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return userLogins;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public List<Comm> getTellHistory(int userId, int offset, int fetchSize) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<Comm> tellHistory = CommUtil.getTellHistory(statement, userId, offset, fetchSize);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return tellHistory;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public void putUserLog(UserLog userLog) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      UserLogUtil.putUserLog(statement, userLog);
      
      statement.close();
      connection.commit();
      connection.close();
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public FeaturedMUDListing getRandomFeaturedMUDListing(Collection<Integer> featuredMUDListingIdCollectionToExclude) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      FeaturedMUDListing featuredMUDListing = FeaturedMUDListingUtil.getRandomFeaturedMUDListing(statement, featuredMUDListingIdCollectionToExclude);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return featuredMUDListing;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public HelpFile getHelpFile(int helpFileId) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      HelpFile helpFile = HelpUtil.getHelpFile(statement, helpFileId, false);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return helpFile;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }

  public HelpFile getHelpFileByModRewrittenName(String modRewrittenName, Integer parentId) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      HelpFile helpFile = HelpUtil.getHelpFileByModRewrittenName(statement, modRewrittenName, parentId, false);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return helpFile;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }

  public List<HelpFile> getHelpFileChain(int helpFileId) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<HelpFile> helpFileChain = HelpUtil.getHelpFileChain(statement, helpFileId);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return helpFileChain;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public List<HelpFile> getHelpFilesByParentId(Integer parentId) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<HelpFile> helpFiles = HelpUtil.getHelpFilesByParentId(statement, parentId);
      
      statement.close();
      connection.commit();
      connection.close();
      
      return helpFiles;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
}
